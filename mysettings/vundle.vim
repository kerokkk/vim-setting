set nocompatible
filetype off

set runtimepath+=$VIMFILES/bundle/vundle
call vundle#rc()

" Vundleで管理するプラグイン
Bundle 'Shougo/neobundle.vim'
Bundle 'Shougo/vimproc'
Bundle 'Shougo/unite.vim'
Bundle 'Shougo/neocomplete'
Bundle 'Shougo/neosnippet'
Bundle 'Shougo/neosnippet-snippets'
Bundle 'Shougo/neomru.vim'
Bundle 'thinca/vim-singleton'
Bundle 'sjl/gundo.vim'
Bundle 'gtags.vim'

filetype plugin indent on
