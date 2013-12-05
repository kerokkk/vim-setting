set nocompatible
filetype off

set runtimepath+=$VIMFILES/bundle/neobundle.vim
call neobundle#rc(expand($VIMFILES. '/bundle/'))

" Vundleで管理するプラグイン
NeoBundle 'Shougo/neobundle.vim'
NeoBundle 'Shougo/vimproc'
NeoBundle 'Shougo/unite.vim'
NeoBundle 'Shougo/neocomplete'
NeoBundle 'thinca/vim-singleton'
NeoBundle 'sjl/gundo.vim'

filetype plugin indent on
filetype indent on
syntax on
