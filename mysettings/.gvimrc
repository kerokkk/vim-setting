scriptencoding cp932

" �t�H���g�ݒ�
set antialias
if has('win32') || has('win64')
    set guifont=�䂽�ۂ�i�R�[�f�B���O�j:h12
elseif has('unix')
    set guifont=�䂽�ۂ�i�R�[�f�B���O�j\ 12
endif
set ambiwidth=double

" �J�[�\���ݒ�
" IME ON���̃J�[�\���̐F��ݒ�(�ݒ��:��)
highlight CursorIM guibg=Blue guifg=NONE
" �}�����[�h�E�������[�h�ł̃f�t�H���g��IME��Ԑݒ�
set iminsert=0 imsearch=0

" ���j���[�\��
set guioptions=aecmgrb

" �E�B���h�E�̑傫����ۑ�
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
