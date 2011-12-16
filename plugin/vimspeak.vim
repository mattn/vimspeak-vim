let s:libfile = substitute(expand('<sfile>'), '\.vim', '.dll', '')

function! VimSpeak(count, text) range
  let text = a:count == -1 ? a:text : join(a:firstline, a:lastline)
  let text = substitute(text, "\t", "  ", "g")
  if has('gui_running')
    let winpos = { 'x':getwinposx(), 'y':getwinposy() }
  else
    redir => out | silent! winpos | redir END
    let mpos = matchlist(out, '^[^:]\+: X \(-\?\d\+\), Y \(-\?\d\+\)')
    if len(mpos) == 0 | return | endif
    let winpos = { 'x':mpos[1], 'y':mpos[2] }
  endif
  let text = join([winpos.x,winpos.y,text],"\t")
  call libcall(s:libfile, "Speak", iconv(text, &encoding, "utf-8"))
endfunction

command! -nargs=* -range VimSpeak <line1>,<line2>call VimSpeak(<count>, <q-args>)

" vim:set ts=8 sts=2 sw=2 tw=0:
