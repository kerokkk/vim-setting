" バッファ一覧
nnoremap <silent> <Leader>b     :<C-u>Unite -buffer-name=buffers buffer<CR>
" ファイル一覧
nnoremap <silent> <Leader>f     :<C-u>UniteWithBufferDir -buffer-name=files file file/new bookmark<CR>
nnoremap <silent> <Leader>F     :<C-u>UniteWithBufferDir -buffer-name=files_rec file_rec file/new bookmark<CR>
" レジスタ一覧
nnoremap <silent> <Leader>ur    :<C-u>Unite -buffer-name=register register<CR>
" コマンド一覧
nnoremap <silent> <Leader>uc    :<C-u>Unite -buffer-name=commands command<CR>
" 最近使用したファイル一覧
nnoremap <silent> <Leader>n     :<C-u>Unite -buffer-name=mru file_mru<CR>
" ブックマーク
nnoremap <silent> <Leader>uo    :<C-u>Unite -buffer-name=bookmark bookmark<CR>
" ヤンクリスト
nnoremap <silent> <Leader>uy    :<C-u>Unite -buffer-name=yanks history/yank<CR>
" 常用セット
nnoremap <silent> <Leader>uu    :<C-u>Unite -buffer-name=regular buffer file_mru bookmark<CR>
" 全部乗せ
nnoremap <silent> <Leader>ua    :<C-u>Unite -buffer-name=all buffer file_mru bookmark file<CR>
" ヘルプ
nnoremap <silent> <Leader>uh    :<C-u>Unite -buffer-name=help -start-insert help<CR>
" grep
nnoremap <silent> <Leader>ug    :<C-u>Unite -buffer-name=grep -start-insert grep<CR>
augroup unite
    au!
    " 終了
    au FileType unite nmap <buffer> <Esc> <Plug>(unite_exit)
    " 分割して開く
    au FileType unite nnoremap <silent> <buffer> <expr> <C-s> unite#do_action('split')
    au FileType unite inoremap <silent> <buffer> <expr> <C-s> unite#do_action('split')
    au FileType unite nnoremap <silent> <buffer> <expr> <C-l> unite#do_action('vsplit')
    au FileType unite inoremap <silent> <buffer> <expr> <C-l> unite#do_action('vsplit')
    " C-wでパスの１階層分が削除されるように
    au FileType unite imap <buffer> <C-w> <Plug>(unite_delete_backward_path)
augroup END
"unite_enable_start_insertnite実行後に即入力モードに
let g:unite_enable_start_insert = 1
"高さ設定
let g:unite_winheight=15
"Uniteが設定ファイルを管理するディレクトリ．
let g:unite_data_directory = $HOME . '/.unite'
" ヤンクリストを有効にする
let g:unite_source_history_yank_enable = 1
" 絞込みテキストを入力してから反映されるまでの時間
let g:unite_update_time = 100

" ランチャ設定
let s:unite_source = {
\   "name" : "command_launcher"
\}

function! s:unite_source.gather_candidates(args, context)
    let cmds = {
\       "EqBoot"  : "!EqBoot",
\       "Ns300pKill"      : "!Ns300pKill",
\       "CreateConfigAndMake"    : "!CreateConfigAndMake",
\       "Make Source" : "!make source"
\   }

    return values(map(cmds, "{
\       'word' : v:key,
\       'source' : 'command_launcher',
\       'kind' : 'command',
\       'action__command' : v:val
\   }"))
endfunction

call unite#define_source(s:unite_source)

" 呼び出しのキーマップ
nnoremap <silent> <Leader>c :Unite command_launcher<CR>
