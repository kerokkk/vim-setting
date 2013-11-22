set nocompatible

filetype plugin indent on
syntax on

" �p�X�̐ݒ�
let VIM=$HOME

" �����R�[�h�A���s�R�[�h�ݒ�
set fileencodings=utf-8,cp932,euc-jp
set fileformat=dos

" �^�u�̐ݒ�
set tabstop=4               " �^�u�̉�ʏ�ł̕�
set softtabstop=4           " �^�u�L�[���������Ƃ���~����ʒu
set shiftwidth=4            " >>��<<�̃X�g�b�v����ʒu
set expandtab
set list
set listchars=tab:>-
set backspace=2             " backspace�ŉ��s��C���f���g���폜�ł���悤�ɂ���

" �����ݒ�
set hlsearch
set ignorecase
set smartcase
set wrapscan

" �C���f���g�ݒ�
set autoindent

" ��ʕ\���ݒ�
set number
set novisualbell
set wrap
set laststatus=2            " �X�e�[�^�X���C���̕\��
set statusline=%.50f\ %m%r%h%q%=%l/%L\ %y\ [%{&fenc}]\ [%{&ff}]
set cmdheight=2
set title
syntax enable
colorscheme pyte

" �ҏW�ݒ�
set showmatch               " �J�b�R�̑Ή�������̂��n�C���C�g

" �L�[�}�b�s���O
so $VIMRUNTIME/mysettings/keymap.vim
