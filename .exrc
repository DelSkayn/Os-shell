let s:cpo_save=&cpo
set cpo&vim
inoremap <C-Space> 
imap <Nul> <C-Space>
inoremap <expr> <Up> pumvisible() ? "\" : "\<Up>"
inoremap <expr> <S-Tab> pumvisible() ? "\" : "\<S-Tab>"
inoremap <expr> <Down> pumvisible() ? "\" : "\<Down>"
vmap [% [%m'gv``
nnoremap \d :YcmShowDetailedDiagnostic
vmap ]% ]%m'gv``
vmap a% [%v]%
vmap gx <Plug>NetrwBrowseXVis
nmap gx <Plug>NetrwBrowseX
vnoremap <silent> <Plug>NetrwBrowseXVis :call netrw#BrowseXVis()
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#BrowseX(expand((exists("g:netrw_gx")? g:netrw_gx : '<cfile>')),netrw#CheckIfRemote())
inoremap <expr> 	 pumvisible() ? "\" : "\	"
let &cpo=s:cpo_save
unlet s:cpo_save
set background=dark
set completefunc=youcompleteme#Complete
set completeopt=preview,menuone
set cpoptions=aAceFsB
set expandtab
set fileencodings=ucs-bom,utf-8,default,latin1
set helplang=nl
set omnifunc=youcompleteme#OmniComplete
set runtimepath=~/.config/nvim,~/.vim/bundle/Vundle.vim,~/.vim/bundle/vim-colors-solarized,~/.vim/bundle/nerdtree,~/.vim/bundle/YouCompleteMe,~/.vim/bundle/rust.vim,~/.vim/bundle/racer,~/.vim/bundle/vim-javascript-syntax,~/.vim/bundle/vim-airline,~/.vim/bundle/YCM-Generator,/etc/xdg/nvim,~/.local/share/nvim/site,/usr/share/nvim/site,/usr/share/nvim/site,/usr/local/share/nvim/site,/usr/share/nvim/runtime,/usr/local/share/nvim/site/after,/usr/share/nvim/site/after,/usr/share/nvim/site/after,~/.local/share/nvim/site/after,/etc/xdg/nvim/after,~/.config/nvim/after,~/.vim/bundle/Vundle.vim,~/.vim/bundle/Vundle.vim/after,~/.vim/bundle/vim-colors-solarized/after,~/.vim/bundle/nerdtree/after,~/.vim/bundle/YouCompleteMe/after,~/.vim/bundle/rust.vim/after,~/.vim/bundle/racer/after,~/.vim/bundle/vim-javascript-syntax/after,~/.vim/bundle/vim-airline/after,~/.vim/bundle/YCM-Generator/after
set shiftwidth=4
set shortmess=filnxtToOc
set updatetime=2000
set window=51
" vim: set ft=vim :
