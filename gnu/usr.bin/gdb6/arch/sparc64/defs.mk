# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative-gdb,v 1.1 2006/05/29 19:10:58 nathanw Exp 
# Generated from: NetBSD: mknative.common,v 1.8 2006/05/26 19:17:21 mrg Exp 
#
G_INTERNAL_CFLAGS=     -I. -I${GNUHOSTDIST}/gdb -I${GNUHOSTDIST}/gdb/config -DLOCALEDIR="\"/usr/share/locale\"" -DHAVE_CONFIG_H -I${GNUHOSTDIST}/gdb/../include/opcode -I${GNUHOSTDIST}/gdb/../readline/..  -I../bfd -I${GNUHOSTDIST}/gdb/../bfd -I${GNUHOSTDIST}/gdb/../include  -I../intl -I${GNUHOSTDIST}/gdb/../intl  -DMI_OUT=1 -DTUI=1 -Wimplicit -Wreturn-type -Wcomment -Wtrigraphs -Wformat -Wparentheses -Wpointer-arith -Wformat-nonliteral -Wunused-label -Wunused-function -Wno-pointer-sign 
G_LIBGDB_OBS=sparc64-tdep.o sparc64nbsd-tdep.o  sparc-tdep.o sparcnbsd-tdep.o nbsd-tdep.o  corelow.o solib.o solib-svr4.o ser-base.o ser-unix.o ser-pipe.o ser-tcp.o fork-child.o inf-ptrace.o  nbsd-nat.o sparc64nbsd-nat.o sparc-nat.o bsd-kvm.o  remote.o dcache.o remote-utils.o tracepoint.o ax-general.o ax-gdb.o remote-fileio.o  cli-dump.o  cli-decode.o cli-script.o cli-cmds.o cli-setshow.o cli-utils.o  cli-logging.o  cli-interp.o mi-out.o mi-console.o  mi-cmds.o mi-cmd-env.o mi-cmd-var.o mi-cmd-break.o mi-cmd-stack.o  mi-cmd-file.o mi-cmd-disas.o mi-symbol-cmds.o  mi-interp.o  mi-main.o mi-parse.o mi-getopt.o mi-common.o posix-hdep.o tui-command.o  tui-data.o  tui-disasm.o  tui-file.o tui.o  tui-hooks.o  tui-interp.o  tui-io.o  tui-layout.o  tui-out.o  tui-regs.o  tui-source.o  tui-stack.o  tui-win.o  tui-windata.o  tui-wingeneral.o  tui-winsource.o c-exp.o  cp-name-parser.o  objc-exp.o  ada-exp.o  jv-exp.o  f-exp.o m2-exp.o p-exp.o  version.o  annotate.o  auxv.o  bfd-target.o  blockframe.o breakpoint.o findvar.o regcache.o  charset.o disasm.o dummy-frame.o  source.o value.o eval.o valops.o valarith.o valprint.o printcmd.o  block.o symtab.o symfile.o symmisc.o linespec.o dictionary.o  infcall.o  infcmd.o infrun.o  expprint.o environ.o stack.o thread.o  exceptions.o  inf-child.o  interps.o  main.o  macrotab.o macrocmd.o macroexp.o macroscope.o  event-loop.o event-top.o inf-loop.o completer.o  gdbarch.o arch-utils.o gdbtypes.o osabi.o copying.o  memattr.o mem-break.o target.o parse.o language.o buildsym.o  std-regs.o  signals.o  kod.o kod-cisco.o  gdb-events.o  exec.o bcache.o objfiles.o observer.o minsyms.o maint.o demangle.o  dbxread.o coffread.o coff-pe-read.o elfread.o  dwarfread.o dwarf2read.o mipsread.o stabsread.o corefile.o  dwarf2expr.o dwarf2loc.o dwarf2-frame.o  ada-lang.o c-lang.o f-lang.o objc-lang.o  ui-out.o cli-out.o  varobj.o wrapper.o  jv-lang.o jv-valprint.o jv-typeprint.o  m2-lang.o p-lang.o p-typeprint.o p-valprint.o  scm-exp.o scm-lang.o scm-valprint.o  sentinel-frame.o  complaints.o typeprint.o  ada-typeprint.o c-typeprint.o f-typeprint.o m2-typeprint.o  ada-valprint.o c-valprint.o cp-valprint.o f-valprint.o m2-valprint.o  nlmread.o serial.o mdebugread.o top.o utils.o  ui-file.o  user-regs.o  frame.o frame-unwind.o doublest.o  frame-base.o  gnu-v2-abi.o gnu-v3-abi.o hpacc-abi.o cp-abi.o cp-support.o  cp-namespace.o  reggroups.o regset.o  trad-frame.o  tramp-frame.o  solib.o solib-null.o  prologue-value.o inflow.o    init.o
G_SIM_OBS=
