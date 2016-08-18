scriptencoding cp932

set nocompatible

filetype plugin indent on
syntax on

set shellslash              " windows�Ńp�X��/���g����悤��

"--------------------------------------------------
" �p�X�̐ݒ�
"--------------------------------------------------
if has('win32') || has('win64')
    let $VIMFILES=$HOME. '/vimfiles'
    let $VIMSETTINGS=$VIMFILES. '/mysettings'
else
    let $VIMFILES=$HOME. '/.vim'
    let $VIMSETTINGS=$VIMFILES. '/mysettings'
endif
"--------------------------------------------------
" �����R�[�h�A���s�R�[�h�ݒ�
"--------------------------------------------------
set fileencodings=utf-8,euc-jp,iso-2022-jp-3,iso-2022-jp,euc-jisx0213,ucs-bom,euc-jp,eucjp-ms,cp932
set fileformats=unix,dos
if has('win32') || has('win64')
    set encoding=cp932
else
    set encoding=utf-8
endif



"--------------------------------------------------
" �����ݒ�
"--------------------------------------------------
set hlsearch
set ignorecase
set smartcase
set wrapscan
set history=100

" �O��grep�ݒ�
set grepprg=grep\ -nH


"--------------------------------------------------
" �ҏW�ݒ�
"--------------------------------------------------
" �^�u�̐ݒ�
set tabstop=4               " �^�u�̉�ʏ�ł̕�
set softtabstop=4           " �^�u�L�[���������Ƃ���~����ʒu
set shiftwidth=4            " >>��<<�̃X�g�b�v����ʒu
set expandtab
set list
set listchars=tab:>-
set backspace=2             " backspace�ŉ��s��C���f���g���폜�ł���悤�ɂ���
set smarttab                " tab����������K�؂Ȉʒu�܂ŃC���f���g

" �⊮�ݒ�
set completeopt=menu,menuone,longest

" �N���b�v�{�[�h�ݒ�
if has('unnamedplus')
    set clipboard=unnamedplus
else
    set clipboard=unnamed
endif

" j/k�R�}���h����������̍s�ɏ]���ē��삷��悤�ɂ���
nnoremap j gj
nnoremap k gk

"vimgrep��make��|cw�������Œǉ�(�}�b�`�����t�@�C����QuickFix�ŊJ��)
augroup vimgrep
    au!
    au QuickfixCmdPost vimgrep copen
    au QuickFixCmdPost make copen
augroup END

" Command-line window�̃J�X�^�}�C�Y
nnoremap :   q:
xnoremap :   q:
nnoremap /   q/
xnoremap /   q/
nnoremap ?   q?
xnoremap ?   q?
augroup clwindow
    au!
    au CmdwinEnter * call s:init_cmdwin()
augroup END
function! s:init_cmdwin()
    nnoremap <buffer> <Esc> :<C-u>quit<CR>
    inoremap <buffer> <expr><CR> pumvisible() ? "\<C-y>\<CR>" : "\<CR>"
    startinsert!
endfunction

" ���݊J���Ă���t�@�C���ɃJ�����g�f�B���N�g�����ڂ�
augroup lcd
	au!
	au BufEnter * lcd %:p:h
augroup END

" ���݂��Ȃ��t�@�C����gf�ŊJ����悤�ɂ���
nnoremap gf :e <cfile><CR>

set showmatch               " �J�b�R�̑Ή�������̂��n�C���C�g
set autoindent
set wildmenu                " �R�}���h���C���ł̕⊮������
set noautoread                " �O���ŕύX���ꂽ�Ƃ��͎����œǂݍ��܂Ȃ�
set virtualedit=block       " ��`�͈͑I�����Ƀe�L�X�g�̊O�̗̈�܂őI���ł���悤�ɂ���


"--------------------------------------------------
" ��ʕ\���ݒ�
"--------------------------------------------------
" �x���̐ݒ�
set noerrorbells
set novisualbell
set t_vb=

" �X�e�[�^�X���C��
set laststatus=2            " �X�e�[�^�X���C���̕\��
set statusline=%.50f\ %m%h%q%=%l/%L\ %y\ [%{&fenc}]\ [%{&ff}]
set cmdheight=2

set number
set wrap
set title
syntax enable
colorscheme pyte


"--------------------------------------------------
" �o�b�N�A�b�v�̐ݒ�
"--------------------------------------------------
set backup
set writebackup

" vim���ċN�����Ă����삷��A���h�D
if version >= 703
    set undofile
endif

" .swp�t�@�C��,�o�b�N�A�b�v�t�@�C���̏o�͐�
if isdirectory($HOME. "/.vimwork") == 0
    call mkdir($HOME. "/.vimwork")
endif
set directory=$HOME/.vimwork
set backupdir=$HOME/.vimwork

" �A���h�D�t�@�C���̏o�͐�
if version >= 703
    set undodir=$HOME/.vimwork
endif


" TINA�J���p�ݒ�
so $VIMSETTINGS/tinadev.vim

" �L�[�}�b�s���O
so $VIMSETTINGS/keymap.vim

" �v���O�C���ݒ�
so $VIMSETTINGS/plugin.vim
