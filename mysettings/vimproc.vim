if has('win32')
    let g:vimproc_dll_path=$VIMFILES. '/bundle/vimproc/autoload/vimproc_win32.dll'
elseif has('win64')
    let g:vimproc_dll_path=$VIMFILES. '/bundle/vimproc/autoload/vimproc_win64.dll'
elseif has('unix')
    let g:vimproc_dll_path=$VIMFILES. '/bundle/vimproc/autoload/vimproc_unix.so'
endif
