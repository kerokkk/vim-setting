let mapleader = " "

" インデントを簡単にする
nnoremap > >><Esc>
nnoremap < <<<Esc>
vnoremap > >><Esc>
vnoremap < <<<Esc>

" QuickFixの項目を移動
nnoremap <C-n> :cn<CR>
nnoremap <C-p> :cp<CR>

" .vimrcを簡単に開く
nnoremap <Leader>. :e $VIMFILES/mysettings/.vimrc<CR>
nnoremap <Leader>, :e $VIMFILES/mysettings/.gvimrc<CR>

" F5で設定を読み込む
augroup vimload
    au!
    au FileType vim nnoremap <F5> :w<CR>:so %<CR>
augroup END

" タブ設定
nnoremap <C-t>      :tabnew<CR>
nnoremap <C-Tab>    :tabnext<CR>
nnoremap <S-Tab>    :tabprev<CR>
nnoremap QQ         :tabclose<CR>

nnoremap <Tab>  <C-^>

" コード整形
nnoremap <C-\>  ggVG=2<C-o>
