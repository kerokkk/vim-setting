set nocompatible

filetype plugin indent on
syntax on

" パスの設定
let VIM=$HOME

" 文字コード、改行コード設定
set fileencodings=utf-8,cp932,euc-jp
set fileformat=dos

" タブの設定
set tabstop=4               " タブの画面上での幅
set softtabstop=4           " タブキーを押したとき停止する位置
set shiftwidth=4            " >>や<<のストップする位置
set expandtab
set list
set listchars=tab:>-
set backspace=2             " backspaceで改行やインデントを削除できるようにする

" 検索設定
set hlsearch
set ignorecase
set smartcase
set wrapscan

" インデント設定
set autoindent

" 画面表示設定
set number
set novisualbell
set wrap
set laststatus=2            " ステータスラインの表示
set statusline=%.50f\ %m%r%h%q%=%l/%L\ %y\ [%{&fenc}]\ [%{&ff}]
set cmdheight=2
set title
syntax enable
colorscheme pyte

" 編集設定
set showmatch               " カッコの対応するものをハイライト

" キーマッピング
so $VIMRUNTIME/mysettings/keymap.vim
