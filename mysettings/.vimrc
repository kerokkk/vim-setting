scriptencoding cp932

set nocompatible

filetype plugin indent on
syntax on

set shellslash              " windowsでパスに/を使えるように

"--------------------------------------------------
" パスの設定
"--------------------------------------------------
if has('win32') || has('win64')
    let $VIMFILES=$HOME. '/vimfiles'
    let $VIMSETTINGS=$VIMFILES. '/mysettings'
else
    let $VIMFILES=$HOME. '/.vim'
    let $VIMSETTINGS=$VIMFILES. '/mysettings'
endif
"--------------------------------------------------
" 文字コード、改行コード設定
"--------------------------------------------------
set fileencodings=utf-8,euc-jp,iso-2022-jp-3,iso-2022-jp,euc-jisx0213,ucs-bom,euc-jp,eucjp-ms,cp932
set fileformats=unix,dos
if has('win32') || has('win64')
    set encoding=cp932
else
    set encoding=utf-8
endif



"--------------------------------------------------
" 検索設定
"--------------------------------------------------
set hlsearch
set ignorecase
set smartcase
set wrapscan
set history=100

" 外部grep設定
set grepprg=grep\ -nH


"--------------------------------------------------
" 編集設定
"--------------------------------------------------
" タブの設定
set tabstop=4               " タブの画面上での幅
set softtabstop=4           " タブキーを押したとき停止する位置
set shiftwidth=4            " >>や<<のストップする位置
set expandtab
set list
set listchars=tab:>-
set backspace=2             " backspaceで改行やインデントを削除できるようにする
set smarttab                " tabを押したら適切な位置までインデント

" 補完設定
set completeopt=menu,menuone,longest

" クリップボード設定
if has('unnamedplus')
    set clipboard=unnamedplus
else
    set clipboard=unnamed
endif

" j/kコマンドが見かけ上の行に従って動作するようにする
nnoremap j gj
nnoremap k gk

"vimgrepやmakeの|cwを自動で追加(マッチしたファイルをQuickFixで開く)
augroup vimgrep
    au!
    au QuickfixCmdPost vimgrep copen
    au QuickFixCmdPost make copen
augroup END

" Command-line windowのカスタマイズ
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

" 現在開いているファイルにカレントディレクトリを移す
augroup lcd
	au!
	au BufEnter * lcd %:p:h
augroup END

" 存在しないファイルもgfで開けるようにする
nnoremap gf :e <cfile><CR>

set showmatch               " カッコの対応するものをハイライト
set autoindent
set wildmenu                " コマンドラインでの補完を強化
set noautoread                " 外部で変更されたときは自動で読み込まない
set virtualedit=block       " 矩形範囲選択時にテキストの外の領域まで選択できるようにする


"--------------------------------------------------
" 画面表示設定
"--------------------------------------------------
" ベルの設定
set noerrorbells
set novisualbell
set t_vb=

" ステータスライン
set laststatus=2            " ステータスラインの表示
set statusline=%.50f\ %m%h%q%=%l/%L\ %y\ [%{&fenc}]\ [%{&ff}]
set cmdheight=2

set number
set wrap
set title
syntax enable
colorscheme pyte


"--------------------------------------------------
" バックアップの設定
"--------------------------------------------------
set backup
set writebackup

" vimが再起動しても動作するアンドゥ
if version >= 703
    set undofile
endif

" .swpファイル,バックアップファイルの出力先
if isdirectory($HOME. "/.vimwork") == 0
    call mkdir($HOME. "/.vimwork")
endif
set directory=$HOME/.vimwork
set backupdir=$HOME/.vimwork

" アンドゥファイルの出力先
if version >= 703
    set undodir=$HOME/.vimwork
endif


" TINA開発用設定
so $VIMSETTINGS/tinadev.vim

" キーマッピング
so $VIMSETTINGS/keymap.vim

" プラグイン設定
so $VIMSETTINGS/plugin.vim
