scriptencoding cp932

" �t�H���g�ݒ�
set antialias
if has('win32') || has('win64')
    set guifont=�䂽�ۂ�i�R�[�f�B���O�j:h11
elseif has('unix')
    set guifont=�䂽�ۂ�i�R�[�f�B���O�j\ 11
endif
set ambiwidth=double

" �J�[�\���ݒ�
" IME ON���̃J�[�\���̐F��ݒ�(�ݒ��:��)
highlight CursorIM guibg=Blue guifg=NONE
" �}�����[�h�E�������[�h�ł̃f�t�H���g��IME��Ԑݒ�
set iminsert=0 imsearch=0

" ���j���[�\��
set guioptions=aecmgrb

" �ő剻���ĊJ��
if has('win32') || has('win64')
    augroup gui
        au!
        au GUIEnter * simalt ~x
    augroup END
endif
