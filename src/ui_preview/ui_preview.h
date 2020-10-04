/**
 * File:   ui_preview.h
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

#ifndef TK_UI_PREVIEW_H
#define TK_UI_PREVIEW_H

#include "httpd/httpd.h"

BEGIN_C_DECLS

/**
 * @class ui_preview_t
 * @annotation ["fake"]
 * UI Preview。
 */

/**
 * @method ui_preview_start
 * 启动测试代理。
 * @param {int} port 监听端口号。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t ui_preview_start(int port);

/**
 * @method ui_preview_stop
 * 停止测试代理。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t ui_preview_stop(void);

END_C_DECLS

#endif /*TK_UI_PREVIEW_H*/
