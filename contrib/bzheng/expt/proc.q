.proc:(enlist `)!(enlist (::));
.proc,:2:[`proc;(`proc;1)][];
.proc.__log:{-1 string[.z.p]," UTC\t",x};
.proc.start:{[x;y;z]
    .proc.__log "Client ",string[x],"@",string[y]," connected to child pid ",string[z];
    };
.proc.term:{[x;y]
    .proc.__log "Handling pid ",string[x],", status=",string[y];
    };
.proc.end:{[x;y;z]
    .proc.__log "Child pid ",string[x]," ",string[y],", status=",string[z];
    };

.proc.act:{
    .z.po:{u:.z.u;h:` sv `$string `int$0x0 vs .z.a;.proc.start[u;h] .proc.child["hclose .z.w;";"system each (\"x .z.po\";\"p 0\";\"T 300\");"]};
    .z.ts:.proc.clean;system "t 1000"; // 
    };

\

q).proc.act[]

2015.04.24D01:52:25.212721000 UTC       Client c@127.0.0.1 connected to child pid 21302
2015.04.24D01:52:28.065624000 UTC       Handling pid 21302, status=0
2015.04.24D01:52:28.065656000 UTC       Child pid 21302 exited, status=0
2015.04.24D01:52:30.817383000 UTC       Client c@127.0.0.1 connected to child pid 21304
2015.04.24D01:52:48.278366000 UTC       Handling pid 21304, status=9
2015.04.24D01:52:48.278434000 UTC       Child pid 21304 killed by signal, status=9
    