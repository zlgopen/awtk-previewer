import os
import scripts.app_helper as app

DEPENDS_LIBS = [
  {
    "root" : '../awtk-restful-httpd',
    'shared_libs': ['httpd'],
    'static_libs': []
  }
]

helper = app.Helper(ARGUMENTS);
helper.set_dll_def('src/ui_preview.def').set_libs(['ui_preview']).set_deps(DEPENDS_LIBS).call(DefaultEnvironment)

SConscriptFiles = ['src/ui_preview/SConscript', 'src/app/SConscript']
SConscript(SConscriptFiles)
