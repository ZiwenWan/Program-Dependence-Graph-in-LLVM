; ModuleID = 'example.k1..c'
source_filename = "example.k1..c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.node = type { %struct.device_operations*, %struct.node* }
%struct.device_operations = type { void (%struct.device_operations*)*, i32 }

@devops_list = common global %struct.node* null, align 8, !dbg !0
@.str = private unnamed_addr constant [25 x i8] c"Add Devops to the list:\0A\00", align 1
@devops = internal global %struct.device_operations { void (%struct.device_operations*)* @mimix_open, i32 0 }, align 8, !dbg !26

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @init(%struct.node*) #0 !dbg !33 {
  %2 = alloca %struct.node*, align 8
  store %struct.node* %0, %struct.node** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.node** %2, metadata !36, metadata !37), !dbg !38
  store %struct.node* null, %struct.node** %2, align 8, !dbg !39
  ret void, !dbg !40
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.node* @add(%struct.node*, %struct.device_operations*) #0 !dbg !41 {
  %3 = alloca %struct.node*, align 8
  %4 = alloca %struct.device_operations*, align 8
  %5 = alloca %struct.node*, align 8
  store %struct.node* %0, %struct.node** %3, align 8
  call void @llvm.dbg.declare(metadata %struct.node** %3, metadata !44, metadata !37), !dbg !45
  store %struct.device_operations* %1, %struct.device_operations** %4, align 8
  call void @llvm.dbg.declare(metadata %struct.device_operations** %4, metadata !46, metadata !37), !dbg !47
  call void @llvm.dbg.declare(metadata %struct.node** %5, metadata !48, metadata !37), !dbg !49
  %6 = call i8* @malloc(i64 16) #5, !dbg !50
  %7 = bitcast i8* %6 to %struct.node*, !dbg !51
  store %struct.node* %7, %struct.node** %5, align 8, !dbg !49
  %8 = load %struct.node*, %struct.node** %5, align 8, !dbg !52
  %9 = icmp eq %struct.node* %8, null, !dbg !54
  br i1 %9, label %10, label %11, !dbg !55

; <label>:10:                                     ; preds = %2
  call void @exit(i32 0) #6, !dbg !56
  unreachable, !dbg !56

; <label>:11:                                     ; preds = %2
  %12 = load %struct.device_operations*, %struct.device_operations** %4, align 8, !dbg !58
  %13 = load %struct.node*, %struct.node** %5, align 8, !dbg !59
  %14 = getelementptr inbounds %struct.node, %struct.node* %13, i32 0, i32 0, !dbg !60
  store %struct.device_operations* %12, %struct.device_operations** %14, align 8, !dbg !61
  %15 = load %struct.node*, %struct.node** %3, align 8, !dbg !62
  %16 = load %struct.node*, %struct.node** %5, align 8, !dbg !63
  %17 = getelementptr inbounds %struct.node, %struct.node* %16, i32 0, i32 1, !dbg !64
  store %struct.node* %15, %struct.node** %17, align 8, !dbg !65
  %18 = load %struct.node*, %struct.node** %5, align 8, !dbg !66
  store %struct.node* %18, %struct.node** %3, align 8, !dbg !67
  %19 = load %struct.node*, %struct.node** %3, align 8, !dbg !68
  ret %struct.node* %19, !dbg !69
}

; Function Attrs: allocsize(0)
declare i8* @malloc(i64) #2

; Function Attrs: noreturn
declare void @exit(i32) #3

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @reg_devops(void (%struct.device_operations*)*) #0 !dbg !70 {
  %2 = alloca void (%struct.device_operations*)*, align 8
  store void (%struct.device_operations*)* %0, void (%struct.device_operations*)** %2, align 8
  call void @llvm.dbg.declare(metadata void (%struct.device_operations*)** %2, metadata !76, metadata !37), !dbg !77
  %3 = load void (%struct.device_operations*)*, void (%struct.device_operations*)** %2, align 8, !dbg !78
  %4 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !79
  %5 = getelementptr inbounds %struct.node, %struct.node* %4, i32 0, i32 0, !dbg !80
  %6 = load %struct.device_operations*, %struct.device_operations** %5, align 8, !dbg !80
  call void %3(%struct.device_operations* %6), !dbg !78
  ret void, !dbg !81
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !82 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str, i32 0, i32 0)), !dbg !85
  %3 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !86
  call void @init(%struct.node* %3), !dbg !87
  %4 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !88
  %5 = call %struct.node* @add(%struct.node* %4, %struct.device_operations* @devops), !dbg !89
  store %struct.node* %5, %struct.node** @devops_list, align 8, !dbg !90
  %6 = load %struct.node*, %struct.node** @devops_list, align 8, !dbg !91
  %7 = getelementptr inbounds %struct.node, %struct.node* %6, i32 0, i32 0, !dbg !92
  %8 = load %struct.device_operations*, %struct.device_operations** %7, align 8, !dbg !92
  %9 = getelementptr inbounds %struct.device_operations, %struct.device_operations* %8, i32 0, i32 0, !dbg !93
  %10 = load void (%struct.device_operations*)*, void (%struct.device_operations*)** %9, align 8, !dbg !93
  call void @reg_devops(void (%struct.device_operations*)* %10), !dbg !94
  ret i32 0, !dbg !95
}

declare i32 @printf(i8*, ...) #4

; Function Attrs: noinline nounwind optnone ssp uwtable
define internal void @mimix_open(%struct.device_operations*) #0 !dbg !96 {
  %2 = alloca %struct.device_operations*, align 8
  store %struct.device_operations* %0, %struct.device_operations** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device_operations** %2, metadata !97, metadata !37), !dbg !98
  %3 = load %struct.device_operations*, %struct.device_operations** %2, align 8, !dbg !99
  %4 = getelementptr inbounds %struct.device_operations, %struct.device_operations* %3, i32 0, i32 1, !dbg !100
  store i32 0, i32* %4, align 8, !dbg !101
  ret void, !dbg !102
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { allocsize(0) }
attributes #6 = { noreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!28, !29, !30, !31}
!llvm.ident = !{!32}

!0 = !DIGlobalVariableExpression(var: !1)
!1 = distinct !DIGlobalVariable(name: "devops_list", scope: !2, file: !3, line: 18, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !5, globals: !25)
!3 = !DIFile(filename: "example.k1..c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/idl_gen_test")
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
!26 = !DIGlobalVariableExpression(var: !27)
!27 = distinct !DIGlobalVariable(name: "devops", scope: !2, file: !3, line: 43, type: !13, isLocal: true, isDefinition: true)
!28 = !{i32 2, !"Dwarf Version", i32 4}
!29 = !{i32 2, !"Debug Info Version", i32 3}
!30 = !{i32 1, !"wchar_size", i32 4}
!31 = !{i32 7, !"PIC Level", i32 2}
!32 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!33 = distinct !DISubprogram(name: "init", scope: !3, file: !3, line: 20, type: !34, isLocal: false, isDefinition: true, scopeLine: 20, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!34 = !DISubroutineType(types: !35)
!35 = !{null, !6}
!36 = !DILocalVariable(name: "head", arg: 1, scope: !33, file: !3, line: 20, type: !6)
!37 = !DIExpression()
!38 = !DILocation(line: 20, column: 17, scope: !33)
!39 = !DILocation(line: 21, column: 10, scope: !33)
!40 = !DILocation(line: 22, column: 1, scope: !33)
!41 = distinct !DISubprogram(name: "add", scope: !3, file: !3, line: 24, type: !42, isLocal: false, isDefinition: true, scopeLine: 24, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!42 = !DISubroutineType(types: !43)
!43 = !{!6, !6, !11}
!44 = !DILocalVariable(name: "node", arg: 1, scope: !41, file: !3, line: 24, type: !6)
!45 = !DILocation(line: 24, column: 17, scope: !41)
!46 = !DILocalVariable(name: "devops_option", arg: 2, scope: !41, file: !3, line: 24, type: !11)
!47 = !DILocation(line: 24, column: 34, scope: !41)
!48 = !DILocalVariable(name: "temp", scope: !41, file: !3, line: 25, type: !6)
!49 = !DILocation(line: 25, column: 11, scope: !41)
!50 = !DILocation(line: 25, column: 26, scope: !41)
!51 = !DILocation(line: 25, column: 18, scope: !41)
!52 = !DILocation(line: 26, column: 9, scope: !53)
!53 = distinct !DILexicalBlock(scope: !41, file: !3, line: 26, column: 9)
!54 = !DILocation(line: 26, column: 14, scope: !53)
!55 = !DILocation(line: 26, column: 9, scope: !41)
!56 = !DILocation(line: 27, column: 9, scope: !57)
!57 = distinct !DILexicalBlock(scope: !53, file: !3, line: 26, column: 23)
!58 = !DILocation(line: 31, column: 20, scope: !41)
!59 = !DILocation(line: 31, column: 5, scope: !41)
!60 = !DILocation(line: 31, column: 11, scope: !41)
!61 = !DILocation(line: 31, column: 18, scope: !41)
!62 = !DILocation(line: 34, column: 18, scope: !41)
!63 = !DILocation(line: 34, column: 5, scope: !41)
!64 = !DILocation(line: 34, column: 11, scope: !41)
!65 = !DILocation(line: 34, column: 16, scope: !41)
!66 = !DILocation(line: 36, column: 12, scope: !41)
!67 = !DILocation(line: 36, column: 10, scope: !41)
!68 = !DILocation(line: 37, column: 12, scope: !41)
!69 = !DILocation(line: 37, column: 5, scope: !41)
!70 = distinct !DISubprogram(name: "reg_devops", scope: !3, file: !3, line: 47, type: !71, isLocal: false, isDefinition: true, scopeLine: 47, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!71 = !DISubroutineType(types: !72)
!72 = !{null, !73}
!73 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !74, size: 64)
!74 = !DISubroutineType(types: !75)
!75 = !{null, !11}
!76 = !DILocalVariable(name: "fp", arg: 1, scope: !70, file: !3, line: 47, type: !73)
!77 = !DILocation(line: 47, column: 23, scope: !70)
!78 = !DILocation(line: 48, column: 6, scope: !70)
!79 = !DILocation(line: 48, column: 9, scope: !70)
!80 = !DILocation(line: 48, column: 22, scope: !70)
!81 = !DILocation(line: 49, column: 1, scope: !70)
!82 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 51, type: !83, isLocal: false, isDefinition: true, scopeLine: 51, isOptimized: false, unit: !2, variables: !4)
!83 = !DISubroutineType(types: !84)
!84 = !{!21}
!85 = !DILocation(line: 52, column: 5, scope: !82)
!86 = !DILocation(line: 53, column: 10, scope: !82)
!87 = !DILocation(line: 53, column: 5, scope: !82)
!88 = !DILocation(line: 54, column: 23, scope: !82)
!89 = !DILocation(line: 54, column: 19, scope: !82)
!90 = !DILocation(line: 54, column: 17, scope: !82)
!91 = !DILocation(line: 55, column: 16, scope: !82)
!92 = !DILocation(line: 55, column: 29, scope: !82)
!93 = !DILocation(line: 55, column: 37, scope: !82)
!94 = !DILocation(line: 55, column: 5, scope: !82)
!95 = !DILocation(line: 56, column: 5, scope: !82)
!96 = distinct !DISubprogram(name: "mimix_open", scope: !3, file: !3, line: 40, type: !17, isLocal: true, isDefinition: true, scopeLine: 40, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!97 = !DILocalVariable(name: "devops", arg: 1, scope: !96, file: !3, line: 40, type: !19)
!98 = !DILocation(line: 40, column: 50, scope: !96)
!99 = !DILocation(line: 41, column: 2, scope: !96)
!100 = !DILocation(line: 41, column: 10, scope: !96)
!101 = !DILocation(line: 41, column: 32, scope: !96)
!102 = !DILocation(line: 42, column: 1, scope: !96)
