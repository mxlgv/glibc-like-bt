; SPDX-License-Identifier: MIT
; Copyright (c) 2024 Maxim Logaev

        global  GlibcBt_GetFrameAddr
        global  _GlibcBt_GetFrameAddr

        section .text

_GlibcBt_GetFrameAddr:
GlibcBt_GetFrameAddr:
%if __BITS__ = 64
        mov     rax, rbp 
%else
        mov     eax, ebp
%endif
        ret
