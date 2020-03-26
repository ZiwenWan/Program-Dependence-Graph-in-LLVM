; ModuleID = 'example.k1.c'
source_filename = "example.k1.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

%struct.node = type { %struct.device_operations*, %struct.node* }
%struct.device_operations = type { void (%struct.device_operations*)*, i32 }

@devops_list = common global %struct.node* null, align 8, !dbg !0
@.str = private unnamed_addr constant [25 x i8] c"Add Devops to the list:\0A\00", align 1
@devops = internal global %struct.device_operations { void (%struct.device_operations*)* @mimix_open, i32 0 }, align 8, !dbg !26

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @init(%struct.node*) #0 !dbg !33 {
  %2 = alloca %struct.node*, align 8
  store %struct.node* %0, %struct.node** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.node** %2, metadata !36, metadata !DIExpression()), !dbg !37
  store %struct.node* null, %struct.node** %2, align 8, !dbg !38
  ret void, !dbg !39
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.node* @add(%struct.node*, %struct.device_operations*) #0 !dbg !40 {
  %3 = alloca %struct.node*, align 8
  %4 = alloca %struct.device_operations*, align 8
  %5 = alloca %struct.node*, align 8
  store %struct.node* %0, %struct.node** %3, align 8
  call void @llvm.dbg.declare(metadata %struct.node** %3, metadata !43, metadata !DIExpression()), !dbg !44
  store %struct.device_operations* %1, %struct.device_operations** %4, align 8
  call void @llvm.dbg.declare(metadata %struct.device_operations** %4, metadata !45, metadata !DIExpression()), !dbg !46
  call void @llvm.dbg.declare(metadata %struct.node** %5, metadata !47, metadata !DIExpression()), !dbg !48
  %6 = call i8* @malloc(i64 16) #5, !dbg !49
  %7 = bitcast i8* %6 to %struct.node*, !dbg !50
  store %struct.node* %7, %struct.node** %5, align 8, !dbg !48
  %8 = load %struct.node*, %struct.node** %5, align 8, !dbg !51
  %9 = icmp eq %struct.node* %8, null, !dbg !53
  br i1 %9, label %10, label %11, !dbg !54

10:                                               ; preds = %2
  call void @exit(i32 0) #6, !dbg !55
  unreachable, !dbg !55

11:                                               ; preds = %2
  %12 = load %struct.device_operations*, %struct.device_operations** %4, align 8, !dbg !57
  %13 = load %struct.node*, %struct.node** %5, align 8, !dbg !58
  %14 = getelementptr inbounds %struct.node, %struct.node* %13, i32 0, i32 0, !dbg !59
  store %struct.device_operations* %12, %struct.device_operations** %14, align 8, !dbg !60
  %15 = load %struct.node*, %struct.node** %3, align 8, !dbg !61
  %16 = load %struct.node*, %struct.node** %5, align 8, !dbg !62
  %17 = getelementptr inbounds %struct.node, %struct.node* %16, i32 0, i32 1, !dbg !63
  store %struct.node* %15, %struct.node** %17, align 8, !dbg !64
  %18 = load %struct.node*, %struct.node** %5, align 8, !dbg !65
  store %struct.node* %18, %struct.node** %3, align 8, !dbg !66
  %19 = load %struct.node*, %struct.node** %3, align 8, !dbg !67
  ret %struct.node* %19, !dbg !68
}

; Function Attrs: allocsize(0)
declare i8* @malloc(i64) #2

; Function Attrs: noreturn
declare void @exit(i32) #3

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @reg_devops(void (%struct.device_operations*)*) #0 !dbg !69 {
  %2 = alloca void (%struct.device_operations*)*, align 8
  store void (%struct.device_operations*)* %0, void (%struct.device_operations*)** %2, align 8
  call void @llvm.dbg.declare(metadata void (%struct.device_operations*)** %2, metadata !75, metadata !DIExpression()), !dbg !76
  %3 = load void (%struct.device_operations*)*, void (%struct.device_operations*)** %2, align 8, !dbg !77
  %4 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !78
  %5 = getelementptr inbounds %struct.node, %struct.node* %4, i32 0, i32 0, !dbg !79
  %6 = load %struct.device_operations*, %struct.device_operations** %5, align 8, !dbg !79
  call void %3(%struct.device_operations* %6), !dbg !77
  ret void, !dbg !80
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !81 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str, i64 0, i64 0)), !dbg !84
  %3 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !85
  call void @init(%struct.node* %3), !dbg !86
  %4 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !87
  %5 = call %struct.node* @add(%struct.node* %4, %struct.device_operations* @devops), !dbg !88
  store %struct.node* %5, %struct.node** @devops_list, align 8, !dbg !89
  %6 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !90
  %7 = getelementptr inbounds %struct.node, %struct.node* %6, i32 0, i32 0, !dbg !91
  %8 = load %struct.device_operations*, %struct.device_operations** %7, align 8, !dbg !91
  %9 = getelementptr inbounds %struct.device_operations, %struct.device_operations* %8, i32 0, i32 0, !dbg !92
  %10 = load void (%struct.device_operations*)*, void (%struct.device_operations*)** %9, align 8, !dbg !92
  call void @reg_devops(void (%struct.device_operations*)* %10), !dbg !93
  ret i32 0, !dbg !94
}

declare i32 @printf(i8*, ...) #4

; Function Attrs: noinline nounwind optnone ssp uwtable
define internal void @mimix_open(%struct.device_operations*) #0 !dbg !95 {
  %2 = alloca %struct.device_operations*, align 8
  store %struct.device_operations* %0, %struct.device_operations** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device_operations** %2, metadata !96, metadata !DIExpression()), !dbg !97
  %3 = load %struct.device_operations*, %struct.device_operations** %2, align 8, !dbg !98
  %4 = getelementptr inbounds %struct.device_operations, %struct.device_operations* %3, i32 0, i32 1, !dbg !99
  store i32 0, i32* %4, align 8, !dbg !100
  ret void, !dbg !101
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { allocsize(0) }
attributes #6 = { noreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!28, !29, !30, !31}
!llvm.ident = !{!32}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "devops_list", scope: !2, file: !3, line: 18, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 9.0.0 (tags/RELEASE_900/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !5, globals: !25, nameTableKind: GNU)
!3 = !DIFile(filename: "example.k1.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/idl_gen_test")
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
!27 = distinct !DIGlobalVariable(name: "devops", scope: !2, file: !3, line: 48, type: !13, isLocal: true, isDefinition: true)
!28 = !{i32 2, !"Dwarf Version", i32 4}
!29 = !{i32 2, !"Debug Info Version", i32 3}
!30 = !{i32 1, !"wchar_size", i32 4}
!31 = !{i32 7, !"PIC Level", i32 2}
!32 = !{!"clang version 9.0.0 (tags/RELEASE_900/final)"}
!33 = distinct !DISubprogram(name: "init", scope: !3, file: !3, line: 20, type: !34, scopeLine: 20, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!34 = !DISubroutineType(types: !35)
!35 = !{null, !6}
!36 = !DILocalVariable(name: "head", arg: 1, scope: !33, file: !3, line: 20, type: !6)
!37 = !DILocation(line: 20, column: 17, scope: !33)
!38 = !DILocation(line: 21, column: 10, scope: !33)
!39 = !DILocation(line: 22, column: 1, scope: !33)
!40 = distinct !DISubprogram(name: "add", scope: !3, file: !3, line: 24, type: !41, scopeLine: 24, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!41 = !DISubroutineType(types: !42)
!42 = !{!6, !6, !11}
!43 = !DILocalVariable(name: "node", arg: 1, scope: !40, file: !3, line: 24, type: !6)
!44 = !DILocation(line: 24, column: 17, scope: !40)
!45 = !DILocalVariable(name: "devops_option", arg: 2, scope: !40, file: !3, line: 24, type: !11)
!46 = !DILocation(line: 24, column: 34, scope: !40)
!47 = !DILocalVariable(name: "temp", scope: !40, file: !3, line: 25, type: !6)
!48 = !DILocation(line: 25, column: 11, scope: !40)
!49 = !DILocation(line: 25, column: 26, scope: !40)
!50 = !DILocation(line: 25, column: 18, scope: !40)
!51 = !DILocation(line: 26, column: 9, scope: !52)
!52 = distinct !DILexicalBlock(scope: !40, file: !3, line: 26, column: 9)
!53 = !DILocation(line: 26, column: 14, scope: !52)
!54 = !DILocation(line: 26, column: 9, scope: !40)
!55 = !DILocation(line: 27, column: 9, scope: !56)
!56 = distinct !DILexicalBlock(scope: !52, file: !3, line: 26, column: 23)
!57 = !DILocation(line: 31, column: 20, scope: !40)
!58 = !DILocation(line: 31, column: 5, scope: !40)
!59 = !DILocation(line: 31, column: 11, scope: !40)
!60 = !DILocation(line: 31, column: 18, scope: !40)
!61 = !DILocation(line: 34, column: 18, scope: !40)
!62 = !DILocation(line: 34, column: 5, scope: !40)
!63 = !DILocation(line: 34, column: 11, scope: !40)
!64 = !DILocation(line: 34, column: 16, scope: !40)
!65 = !DILocation(line: 36, column: 12, scope: !40)
!66 = !DILocation(line: 36, column: 10, scope: !40)
!67 = !DILocation(line: 37, column: 12, scope: !40)
!68 = !DILocation(line: 37, column: 5, scope: !40)
!69 = distinct !DISubprogram(name: "reg_devops", scope: !3, file: !3, line: 40, type: !70, scopeLine: 40, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!70 = !DISubroutineType(types: !71)
!71 = !{null, !72}
!72 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !73, size: 64)
!73 = !DISubroutineType(types: !74)
!74 = !{null, !11}
!75 = !DILocalVariable(name: "fp", arg: 1, scope: !69, file: !3, line: 40, type: !72)
!76 = !DILocation(line: 40, column: 23, scope: !69)
!77 = !DILocation(line: 41, column: 6, scope: !69)
!78 = !DILocation(line: 41, column: 9, scope: !69)
!79 = !DILocation(line: 41, column: 22, scope: !69)
!80 = !DILocation(line: 42, column: 1, scope: !69)
!81 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 53, type: !82, scopeLine: 53, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!82 = !DISubroutineType(types: !83)
!83 = !{!21}
!84 = !DILocation(line: 54, column: 5, scope: !81)
!85 = !DILocation(line: 55, column: 10, scope: !81)
!86 = !DILocation(line: 55, column: 5, scope: !81)
!87 = !DILocation(line: 56, column: 23, scope: !81)
!88 = !DILocation(line: 56, column: 19, scope: !81)
!89 = !DILocation(line: 56, column: 17, scope: !81)
!90 = !DILocation(line: 57, column: 16, scope: !81)
!91 = !DILocation(line: 57, column: 29, scope: !81)
!92 = !DILocation(line: 57, column: 37, scope: !81)
!93 = !DILocation(line: 57, column: 5, scope: !81)
!94 = !DILocation(line: 58, column: 5, scope: !81)
!95 = distinct !DISubprogram(name: "mimix_open", scope: !3, file: !3, line: 44, type: !17, scopeLine: 44, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !2, retainedNodes: !4)
!96 = !DILocalVariable(name: "devops", arg: 1, scope: !95, file: !3, line: 44, type: !19)
!97 = !DILocation(line: 44, column: 50, scope: !95)
!98 = !DILocation(line: 45, column: 2, scope: !95)
!99 = !DILocation(line: 45, column: 10, scope: !95)
!100 = !DILocation(line: 45, column: 32, scope: !95)
!101 = !DILocation(line: 46, column: 1, scope: !95)
