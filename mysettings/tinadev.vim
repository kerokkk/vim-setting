" TINAのログファイル用設定
augroup tinalog 
au!
    au  BufNewFile,BufRead *Log.txt     setfiletype tinalog
    au  BufNewFile,BufRead *Log.txt     set tabstop=8
augroup END

nnoremap    <F1>    :<C-u>!Ns300pKill
nnoremap    <F2>    :<C-u>!sh CreateConfigAndMake.sh
