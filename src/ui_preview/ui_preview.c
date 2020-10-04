/**
 * File:   ui_preview.c
 * Author: AWTK Develop Team
 * Brief:  ui_preview
 *
 * Copyright (c) 2020 - 2020  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2020-08-02 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/fs.h"
#include "tkc/mem.h"
#include "tkc/utils.h"
#include "base/input_method.h"
#include "httpd/http_route.h"
#include "httpd/http_parser.h"

#include "ui_preview.h"
#include "awtk_global.h"
#include "base/window.h"
#include "widgets/view.h"
#include "base/system_info.h"
#include "base/window_manager.h"
#include "ui_loader/ui_serializer.h"
#include "ui_loader/ui_loader_xml.h"
#include "ui_loader/ui_builder_default.h"

#define STR_XML "xml"
#define STR_THEME "theme"
#define STR_WIDTH "width"
#define STR_HEIGHT "height"
#define STR_STATUS "status"
#define STR_COUNTRY "country"
#define STR_LANGUAGE "language"
#define STR_APP_ROOT "app_root"

#define STB_IMAGE_STATIC 1
#define STBI_WRITE_NO_STDIO 1
#define STB_IMAGE_WRITE_STATIC 1
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FREE TKMEM_FREE
#define STBI_MALLOC TKMEM_ALLOC
#define STBI_REALLOC(p, s) TKMEM_REALLOC(p, s)

#include "stb/stb_image_write.h"

static httpd_t* s_httpd;

typedef struct _app_info_t {
  widget_t* widget;
} app_info_t;

static app_info_t* app_info_create(void) {
  return TKMEM_ZALLOC(app_info_t);
}

static ret_t app_info_destroy(app_info_t* app) {
  return_value_if_fail(app != NULL, RET_BAD_PARAMS);

  if (app->widget != NULL) {
    widget_destroy(app->widget);
  }
  memset(app, 0x00, sizeof(*app));
  TKMEM_FREE(app);

  return RET_OK;
}

static ret_t ui_preview_on_get_screenshot(http_connection_t* c) {
  int32_t len = 0;
  bitmap_t* t = NULL;
  ret_t ret = RET_FAIL;
  uint8_t* tdata = NULL;
  unsigned char* png_data = NULL;
  conf_doc_t* resp = c->resp;
  widget_t* wm = window_manager();
  const char* header = "Content-Type: image/png\r\n";
  widget_t* element = window_manager_get_top_window(wm);
  return_value_if_fail(element != NULL, RET_NOT_FOUND);
  t = widget_take_snapshot(wm);
  return_value_if_fail(t != NULL, RET_BAD_PARAMS);

  tdata = bitmap_lock_buffer_for_write(t);
  png_data = stbi_write_png_to_mem(tdata, t->w * 4, t->w, t->h, 4, &len);
  bitmap_unlock_buffer(t);
  bitmap_destroy(t);

  return_value_if_fail(png_data != NULL, RET_BAD_PARAMS);
  if (http_connection_send(c, 200, header, png_data, len) == RET_OK) {
    ret = RET_STOP;
  }

  STBIW_FREE(png_data);

  return ret;
}

static ret_t ui_preview_on_update_ui(http_connection_t* c) {
  conf_doc_t* resp = c->resp;
  app_info_t* app = (app_info_t*)(s_httpd->user_data);

  int32_t w = conf_doc_get_int(c->req, STR_WIDTH, 0);
  int32_t h = conf_doc_get_int(c->req, STR_HEIGHT, 0);
  const char* xml = conf_doc_get_str(c->req, STR_XML, NULL);
  const char* theme = conf_doc_get_str(c->req, STR_THEME, NULL);
  const char* country = conf_doc_get_str(c->req, STR_COUNTRY, NULL);
  const char* language = conf_doc_get_str(c->req, STR_LANGUAGE, NULL);
  const char* app_root = conf_doc_get_str(c->req, STR_APP_ROOT, NULL);

  return_value_if_fail(xml != NULL, RET_BAD_PARAMS);
  return_value_if_fail(w < 2000 && h < 2000, RET_BAD_PARAMS);

  if (w > 0 && h > 0) {
    window_manager_resize(window_manager(), w, h);
  }

  if (app_root != NULL && path_exist(app_root)) {
    system_info_set_app_root(system_info(), app_root);
  }

  if (theme != NULL) {
    char path[MAX_PATH + 1];
    path_build(path, MAX_PATH, system_info()->app_root, "assets", theme, NULL);

    if (path_exist(path)) {
      /*TODO*/
    }
  }

  if (language != NULL || country != NULL) {
    locale_info_change(locale_info(), language, country);
  }

  if (xml != NULL) {
    ui_loader_t* loader = xml_ui_loader();
    ui_builder_t* builder = ui_builder_default("preview");
    return_value_if_fail(loader != NULL && builder != NULL, RET_BAD_PARAMS);

    ui_loader_load(loader, xml, strlen(xml), builder);

    if (app->widget != NULL) {
      widget_destroy(app->widget);
      app->widget = NULL;
    }

    app->widget = builder->root;
    if (app->widget != NULL) {
      widget_set_prop_str(app->widget, WIDGET_PROP_ANIM_HINT, "");
    }
  }

  conf_doc_set_int(resp, STR_STATUS, 0);

  return RET_OK;
}

static const http_route_entry_t s_ui_preview_routes[] = {
    {HTTP_POST, "/ui", ui_preview_on_update_ui},
    {HTTP_GET, "/screenshot", ui_preview_on_get_screenshot}};

ret_t ui_preview_start(int port) {
  httpd_t* httpd = httpd_create(port, 1);
  return_value_if_fail(httpd != NULL, RET_BAD_PARAMS);

  httpd_set_routes(httpd, s_ui_preview_routes, ARRAY_SIZE(s_ui_preview_routes));
  httpd->user_data = app_info_create();

  s_httpd = httpd;
  return httpd_start(httpd);
}

ret_t ui_preview_stop(void) {
  return_value_if_fail(s_httpd != NULL, RET_BAD_PARAMS);

  app_info_destroy((app_info_t*)(s_httpd->user_data));
  httpd_destroy(s_httpd);
  s_httpd = NULL;

  return RET_OK;
}
