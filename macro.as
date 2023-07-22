;MAIN: mov @r3 ,LENGTH
mcro m1
sub @r1, @r4
bne END
endmcro
;MAIN: .entry LEN
;LEN: .data 2,3,4      ,    5    ,4
m1