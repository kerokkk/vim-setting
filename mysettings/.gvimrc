" フォント設定
set antialias
set guifont=ゆたぽん（コーディング）:h12
set ambiwidth=double

" カーソル設定
" IME ON時のカーソルの色を設定(設定例:紫)
highlight CursorIM guibg=Blue guifg=NONE
" 挿入モード・検索モードでのデフォルトのIME状態設定
set iminsert=0 imsearch=0

" メニュー表示
set guioptions=aecmgrb

" ウィンドウの大きさを保存
let g:save_window_file = expand('~/.vimwinpos')
augroup SaveWindow
  autocmd!
  autocmd VimLeavePre * call s:save_window()
  function! s:save_window()
    let options = [
      \ 'set columns=' . &columns,
      \ 'set lines=' . &lines,
      \ 'winpos ' . getwinposx() . ' ' . getwinposy(),
      \ ]
    call writefile(options, g:save_window_file)
  endfunction
augroup END

if filereadable(g:save_window_file)
  execute 'source' g:save_window_file
endif
