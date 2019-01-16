; ModuleID = 'example.k1.c'
source_filename = "example.k1.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.node = type { %struct.device_operations*, %struct.node* }
%struct.device_operations = type { void (%struct.device_operations*)*, i32 }

@devops_list = common global %struct.node* null, align 8, !dbg !0
@.str = private unnamed_addr constant [25 x i8] c"Add Devops to the list:\0A\00", align 1
@devops = internal global %struct.device_operations { void (%struct.device_operations*)* @mimix_open, i32 0 }, align 8, !dbg !26

; Function Attrs: noinline nounwind optnone uwtable
define void @init(%struct.node* %head) #0 !dbg !32 {
entry:
  %head.addr = alloca %struct.node*, align 8
  store %struct.node* %head, %struct.node** %head.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.node** %head.addr, metadata !35, metadata !DIExpression()), !dbg !36
  store %struct.node* null, %struct.node** %head.addr, align 8, !dbg !37
  ret void, !dbg !38
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define %struct.node* @add(%struct.node* %node, %struct.device_operations* %devops_option) #0 !dbg !39 {
entry:
  %node.addr = alloca %struct.node*, align 8
  %devops_option.addr = alloca %struct.device_operations*, align 8
  %temp = alloca %struct.node*, align 8
  store %struct.node* %node, %struct.node** %node.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.node** %node.addr, metadata !42, metadata !DIExpression()), !dbg !43
  store %struct.device_operations* %devops_option, %struct.device_operations** %devops_option.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.device_operations** %devops_option.addr, metadata !44, metadata !DIExpression()), !dbg !45
  call void @llvm.dbg.declare(metadata %struct.node** %temp, metadata !46, metadata !DIExpression()), !dbg !47
  %call = call noalias i8* @malloc(i64 16) #5, !dbg !48
  %0 = bitcast i8* %call to %struct.node*, !dbg !49
  store %struct.node* %0, %struct.node** %temp, align 8, !dbg !47
  %1 = load %struct.node*, %struct.node** %temp, align 8, !dbg !50
  %cmp = icmp eq %struct.node* %1, null, !dbg !52
  br i1 %cmp, label %if.then, label %if.end, !dbg !53

if.then:                                          ; preds = %entry
  call void @exit(i32 0) #6, !dbg !54
  unreachable, !dbg !54

if.end:                                           ; preds = %entry
  %2 = load %struct.device_operations*, %struct.device_operations** %devops_option.addr, align 8, !dbg !56
  %3 = load %struct.node*, %struct.node** %temp, align 8, !dbg !57
  %devops = getelementptr inbounds %struct.node, %struct.node* %3, i32 0, i32 0, !dbg !58
  store %struct.device_operations* %2, %struct.device_operations** %devops, align 8, !dbg !59
  %4 = load %struct.node*, %struct.node** %node.addr, align 8, !dbg !60
  %5 = load %struct.node*, %struct.node** %temp, align 8, !dbg !61
  %next = getelementptr inbounds %struct.node, %struct.node* %5, i32 0, i32 1, !dbg !62
  store %struct.node* %4, %struct.node** %next, align 8, !dbg !63
  %6 = load %struct.node*, %struct.node** %temp, align 8, !dbg !64
  store %struct.node* %6, %struct.node** %node.addr, align 8, !dbg !65
  %7 = load %struct.node*, %struct.node** %node.addr, align 8, !dbg !66
  ret %struct.node* %7, !dbg !67
}

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #2

; Function Attrs: noreturn nounwind
declare void @exit(i32) #3

; Function Attrs: noinline nounwind optnone uwtable
define void @reg_devops(void (%struct.device_operations*)* %fp) #0 !dbg !68 {
entry:
  %fp.addr = alloca void (%struct.device_operations*)*, align 8
  store void (%struct.device_operations*)* %fp, void (%struct.device_operations*)** %fp.addr, align 8
  call void @llvm.dbg.declare(metadata void (%struct.device_operations*)** %fp.addr, metadata !74, metadata !DIExpression()), !dbg !75
  %0 = load void (%struct.device_operations*)*, void (%struct.device_operations*)** %fp.addr, align 8, !dbg !76
  %1 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !77
  %devops = getelementptr inbounds %struct.node, %struct.node* %1, i32 0, i32 0, !dbg !78
  %2 = load %struct.device_operations*, %struct.device_operations** %devops, align 8, !dbg !78
  call void %0(%struct.device_operations* %2), !dbg !76
  ret void, !dbg !79
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 !dbg !80 {
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str, i32 0, i32 0)), !dbg !83
  %0 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !84
  call void @init(%struct.node* %0), !dbg !85
  %1 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !86
  %call1 = call %struct.node* @add(%struct.node* %1, %struct.device_operations* @devops), !dbg !87
  store %struct.node* %call1, %struct.node** @devops_list, align 8, !dbg !88
  %2 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !89
  %devops = getelementptr inbounds %struct.node, %struct.node* %2, i32 0, i32 0, !dbg !90
  %3 = load %struct.device_operations*, %struct.device_operations** %devops, align 8, !dbg !90
  %open = getelementptr inbounds %struct.device_operations, %struct.device_operations* %3, i32 0, i32 0, !dbg !91
  %4 = load void (%struct.device_operations*)*, void (%struct.device_operations*)** %open, align 8, !dbg !91
  call void @reg_devops(void (%struct.device_operations*)* %4), !dbg !92
  ret i32 0, !dbg !93
}

declare i32 @printf(i8*, ...) #4

; Function Attrs: noinline nounwind optnone uwtable
define internal void @mimix_open(%struct.device_operations* %devops) #0 !dbg !94 {
entry:
  %devops.addr = alloca %struct.device_operations*, align 8
  store %struct.device_operations* %devops, %struct.device_operations** %devops.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.device_operations** %devops.addr, metadata !95, metadata !DIExpression()), !dbg !96
  %0 = load %struct.device_operations*, %struct.device_operations** %devops.addr, align 8, !dbg !97
  %devop_init_registered = getelementptr inbounds %struct.device_operations, %struct.device_operations* %0, i32 0, i32 1, !dbg !98
  store i32 0, i32* %devop_init_registered, align 8, !dbg !99
  ret void, !dbg !100
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind }
attributes #6 = { noreturn nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!28, !29, !30}
!llvm.ident = !{!31}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "devops_list", scope: !2, file: !3, line: 18, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !5, globals: !25)
!3 = !DIFile(filename: "example.k1.c", directory: "/home/yongzhe/llvm-versions/llvm-5.0/lib/Analysis/PDG/test/test_idl_gen")
!4 = !{}
!5 = !{!6, !24}
!6 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !7, size: 64)
!7 = !DIDerivedType(tag: DW_TAG_typedef, name: "NODE", file: !3, line: 16, baseType: !8)
!8 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "node", file: !3, line: 13, size: 128, elements: !9)
!9 = !{!10, !22}
!10 = !DIDerivedType(tag: DW_TAG_member, name: "devops", scope: !8, file: !3, line: 14, baseType: !11, size: 64)
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DIDerivedType(tag: DW_TAG_typedef, name: "devops_ds", file: !3, line: 11, baseType: !13)
!13 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "device_operations", file: !3, line: 8, size: 128, elements: !14)
!14 = !{!15, !20}
!15 = !DIDerivedType(tag: DW_TAG_member, name: "open", scope: !13, file: !3, line: 9, baseType: !16, size: 64)
!16 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64)
!17 = !DISubroutineType(types: !18)
!18 = !{null, !19}
!19 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !13, size: 64)
!20 = !DIDerivedType(tag: DW_TAG_member, name: "devop_init_registered", scope: !13, file: !3, line: 10, baseType: !21, size: 32, offset: 64)
!21 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "next", scope: !8, file: !3, line: 15, baseType: !23, size: 64, offset: 64)
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !8, size: 64)
!24 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!25 = !{!0, !26}
!26 = !DIGlobalVariableExpression(var: !27, expr: !DIExpression())
!27 = distinct !DIGlobalVariable(name: "devops", scope: !2, file: !3, line: 43, type: !13, isLocal: true, isDefinition: true)
!28 = !{i32 2, !"Dwarf Version", i32 4}
!29 = !{i32 2, !"Debug Info Version", i32 3}
!30 = !{i32 1, !"wchar_size", i32 4}
!31 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
!32 = distinct !DISubprogram(name: "init", scope: !3, file: !3, line: 20, type: !33, isLocal: false, isDefinition: true, scopeLine: 20, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!33 = !DISubroutineType(types: !34)
!34 = !{null, !6}
!35 = !DILocalVariable(name: "head", arg: 1, scope: !32, file: !3, line: 20, type: !6)
!36 = !DILocation(line: 20, column: 17, scope: !32)
!37 = !DILocation(line: 21, column: 10, scope: !32)
!38 = !DILocation(line: 22, column: 1, scope: !32)
!39 = distinct !DISubprogram(name: "add", scope: !3, file: !3, line: 24, type: !40, isLocal: false, isDefinition: true, scopeLine: 24, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!40 = !DISubroutineType(types: !41)
!41 = !{!6, !6, !11}
!42 = !DILocalVariable(name: "node", arg: 1, scope: !39, file: !3, line: 24, type: !6)
!43 = !DILocation(line: 24, column: 17, scope: !39)
!44 = !DILocalVariable(name: "devops_option", arg: 2, scope: !39, file: !3, line: 24, type: !11)
!45 = !DILocation(line: 24, column: 34, scope: !39)
!46 = !DILocalVariable(name: "temp", scope: !39, file: !3, line: 25, type: !6)
!47 = !DILocation(line: 25, column: 11, scope: !39)
!48 = !DILocation(line: 25, column: 26, scope: !39)
!49 = !DILocation(line: 25, column: 18, scope: !39)
!50 = !DILocation(line: 26, column: 9, scope: !51)
!51 = distinct !DILexicalBlock(scope: !39, file: !3, line: 26, column: 9)
!52 = !DILocation(line: 26, column: 14, scope: !51)
!53 = !DILocation(line: 26, column: 9, scope: !39)
!54 = !DILocation(line: 27, column: 9, scope: !55)
!55 = distinct !DILexicalBlock(scope: !51, file: !3, line: 26, column: 23)
!56 = !DILocation(line: 31, column: 20, scope: !39)
!57 = !DILocation(line: 31, column: 5, scope: !39)
!58 = !DILocation(line: 31, column: 11, scope: !39)
!59 = !DILocation(line: 31, column: 18, scope: !39)
!60 = !DILocation(line: 34, column: 18, scope: !39)
!61 = !DILocation(line: 34, column: 5, scope: !39)
!62 = !DILocation(line: 34, column: 11, scope: !39)
!63 = !DILocation(line: 34, column: 16, scope: !39)
!64 = !DILocation(line: 36, column: 12, scope: !39)
!65 = !DILocation(line: 36, column: 10, scope: !39)
!66 = !DILocation(line: 37, column: 12, scope: !39)
!67 = !DILocation(line: 37, column: 5, scope: !39)
!68 = distinct !DISubprogram(name: "reg_devops", scope: !3, file: !3, line: 47, type: !69, isLocal: false, isDefinition: true, scopeLine: 47, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!69 = !DISubroutineType(types: !70)
!70 = !{null, !71}
!71 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !72, size: 64)
!72 = !DISubroutineType(types: !73)
!73 = !{null, !11}
!74 = !DILocalVariable(name: "fp", arg: 1, scope: !68, file: !3, line: 47, type: !71)
!75 = !DILocation(line: 47, column: 23, scope: !68)
!76 = !DILocation(line: 48, column: 6, scope: !68)
!77 = !DILocation(line: 48, column: 9, scope: !68)
!78 = !DILocation(line: 48, column: 22, scope: !68)
!79 = !DILocation(line: 49, column: 1, scope: !68)
!80 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 51, type: !81, isLocal: false, isDefinition: true, scopeLine: 51, isOptimized: false, unit: !2, variables: !4)
!81 = !DISubroutineType(types: !82)
!82 = !{!21}
!83 = !DILocation(line: 52, column: 5, scope: !80)
!84 = !DILocation(line: 53, column: 10, scope: !80)
!85 = !DILocation(line: 53, column: 5, scope: !80)
!86 = !DILocation(line: 54, column: 23, scope: !80)
!87 = !DILocation(line: 54, column: 19, scope: !80)
!88 = !DILocation(line: 54, column: 17, scope: !80)
!89 = !DILocation(line: 55, column: 16, scope: !80)
!90 = !DILocation(line: 55, column: 29, scope: !80)
!91 = !DILocation(line: 55, column: 37, scope: !80)
!92 = !DILocation(line: 55, column: 5, scope: !80)
!93 = !DILocation(line: 56, column: 5, scope: !80)
!94 = distinct !DISubprogram(name: "mimix_open", scope: !3, file: !3, line: 40, type: !17, isLocal: true, isDefinition: true, scopeLine: 40, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!95 = !DILocalVariable(name: "devops", arg: 1, scope: !94, file: !3, line: 40, type: !19)
!96 = !DILocation(line: 40, column: 50, scope: !94)
!97 = !DILocation(line: 41, column: 2, scope: !94)
!98 = !DILocation(line: 41, column: 10, scope: !94)
!99 = !DILocation(line: 41, column: 32, scope: !94)
!100 = !DILocation(line: 42, column: 1, scope: !94)
