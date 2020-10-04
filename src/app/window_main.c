#include "awtk.h"
#include "ui_preview/ui_preview.h"

ret_t application_init(void) {
  socket_init();
  ui_preview_start(8000);
  window_open("main");

  return RET_OK;
}

/**
 * 退出
 */
ret_t application_exit(void) {
  log_debug("application_exit\n");
  ui_preview_stop();
  socket_deinit();
  return RET_OK;
}
