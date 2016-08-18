scriptencoding cp932

" フォント設定
set antialias
if has('win32') || has('win64')
    set guifont=ゆたぽん（コーディング）:h11
elseif has('unix')
    set guifont=ゆたぽん（コーディング）\ 11
endif
set ambiwidth=double

" カーソル設定
" IME ON時のカーソルの色を設定(設定例:紫)
highlight CursorIM guibg=Blue guifg=NONE
" 挿入モード・検索モードでのデフォルトのIME状態設定
set iminsert=0 imsearch=0

" メニュー表示
set guioptions=aecmgrb

" 最大化して開く
if has('win32') || has('win64')
    augroup gui
        au!
        au GUIEnter * simalt ~x
    augroup END
endif
