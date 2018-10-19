; ModuleID = 'kernel_example3.c'
source_filename = "kernel_example3.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.node = type { %struct.device*, %struct.node* }
%struct.device = type { void (%struct.device*)*, i8*, i32 }

@device_list_head = common global %struct.node* null, align 8, !dbg !0
@.str = private unnamed_addr constant [11 x i8] c"opened %s\0A\00", align 1
@.str.1 = private unnamed_addr constant [25 x i8] c"Add device to the list:\0A\00", align 1
@dev1 = internal global %struct.device { void (%struct.device*)* @dev1_open, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.2, i32 0, i32 0), i32 0 }, align 8, !dbg !25
@dev2 = internal global %struct.device { void (%struct.device*)* @dev2_open, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.3, i32 0, i32 0), i32 0 }, align 8, !dbg !27
@.str.2 = private unnamed_addr constant [9 x i8] c"device_1\00", align 1
@.str.3 = private unnamed_addr constant [9 x i8] c"device_2\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @init(%struct.node*) #0 !dbg !34 {
  %2 = alloca %struct.node*, align 8
  store %struct.node* %0, %struct.node** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.node** %2, metadata !37, metadata !38), !dbg !39
  store %struct.node* null, %struct.node** %2, align 8, !dbg !40
  ret void, !dbg !41
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @register_device(%struct.device*) #0 !dbg !42 {
  %2 = alloca %struct.device*, align 8
  %3 = alloca %struct.node*, align 8
  store %struct.device* %0, %struct.device** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !43, metadata !38), !dbg !44
  call void @llvm.dbg.declare(metadata %struct.node** %3, metadata !45, metadata !38), !dbg !46
  %4 = call i8* @malloc(i64 16) #4, !dbg !47
  %5 = bitcast i8* %4 to %struct.node*, !dbg !48
  store %struct.node* %5, %struct.node** %3, align 8, !dbg !46
  %6 = load %struct.device*, %struct.device** %2, align 8, !dbg !49
  %7 = load %struct.node*, %struct.node** %3, align 8, !dbg !50
  %8 = getelementptr inbounds %struct.node, %struct.node* %7, i32 0, i32 0, !dbg !51
  store %struct.device* %6, %struct.device** %8, align 8, !dbg !52
  %9 = load %struct.node*, %struct.node** @device_list_head, align 8, !dbg !53
  %10 = load %struct.node*, %struct.node** %3, align 8, !dbg !54
  %11 = getelementptr inbounds %struct.node, %struct.node* %10, i32 0, i32 1, !dbg !55
  store %struct.node* %9, %struct.node** %11, align 8, !dbg !56
  %12 = load %struct.node*, %struct.node** %3, align 8, !dbg !57
  store %struct.node* %12, %struct.node** @device_list_head, align 8, !dbg !58
  ret void, !dbg !59
}

; Function Attrs: allocsize(0)
declare i8* @malloc(i64) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @open(%struct.device*) #0 !dbg !60 {
  %2 = alloca %struct.device*, align 8
  store %struct.device* %0, %struct.device** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !63, metadata !38), !dbg !64
  %3 = load %struct.device*, %struct.device** %2, align 8, !dbg !65
  %4 = getelementptr inbounds %struct.device, %struct.device* %3, i32 0, i32 0, !dbg !66
  %5 = load void (%struct.device*)*, void (%struct.device*)** %4, align 8, !dbg !66
  %6 = load %struct.device*, %struct.device** %2, align 8, !dbg !67
  call void %5(%struct.device* %6), !dbg !65
  %7 = load %struct.device*, %struct.device** %2, align 8, !dbg !68
  %8 = getelementptr inbounds %struct.device, %struct.device* %7, i32 0, i32 1, !dbg !69
  %9 = load i8*, i8** %8, align 8, !dbg !69
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str, i32 0, i32 0), i8* %9), !dbg !70
  ret i32 1, !dbg !71
}

declare i32 @printf(i8*, ...) #3

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.device* @lookup_device(i8*) #0 !dbg !72 {
  %2 = alloca %struct.device*, align 8
  %3 = alloca i8*, align 8
  %4 = alloca %struct.node*, align 8
  store i8* %0, i8** %3, align 8
  call void @llvm.dbg.declare(metadata i8** %3, metadata !75, metadata !38), !dbg !76
  call void @llvm.dbg.declare(metadata %struct.node** %4, metadata !77, metadata !38), !dbg !78
  %5 = load %struct.node*, %struct.node** @device_list_head, align 8, !dbg !79
  store %struct.node* %5, %struct.node** %4, align 8, !dbg !78
  br label %6, !dbg !80

; <label>:6:                                      ; preds = %26, %1
  %7 = load %struct.node*, %struct.node** %4, align 8, !dbg !81
  %8 = icmp ne %struct.node* %7, null, !dbg !82
  br i1 %8, label %9, label %27, !dbg !80

; <label>:9:                                      ; preds = %6
  %10 = load %struct.node*, %struct.node** %4, align 8, !dbg !83
  %11 = getelementptr inbounds %struct.node, %struct.node* %10, i32 0, i32 0, !dbg !86
  %12 = load %struct.device*, %struct.device** %11, align 8, !dbg !86
  %13 = getelementptr inbounds %struct.device, %struct.device* %12, i32 0, i32 1, !dbg !87
  %14 = load i8*, i8** %13, align 8, !dbg !87
  %15 = load i8*, i8** %3, align 8, !dbg !88
  %16 = call i32 @strcmp(i8* %14, i8* %15), !dbg !89
  %17 = icmp eq i32 %16, 0, !dbg !90
  br i1 %17, label %18, label %22, !dbg !91

; <label>:18:                                     ; preds = %9
  %19 = load %struct.node*, %struct.node** %4, align 8, !dbg !92
  %20 = getelementptr inbounds %struct.node, %struct.node* %19, i32 0, i32 0, !dbg !93
  %21 = load %struct.device*, %struct.device** %20, align 8, !dbg !93
  store %struct.device* %21, %struct.device** %2, align 8, !dbg !94
  br label %28, !dbg !94

; <label>:22:                                     ; preds = %9
  %23 = load %struct.node*, %struct.node** %4, align 8, !dbg !95
  %24 = getelementptr inbounds %struct.node, %struct.node* %23, i32 0, i32 1, !dbg !96
  %25 = load %struct.node*, %struct.node** %24, align 8, !dbg !96
  store %struct.node* %25, %struct.node** %4, align 8, !dbg !97
  br label %26

; <label>:26:                                     ; preds = %22
  br label %6, !dbg !80, !llvm.loop !98

; <label>:27:                                     ; preds = %6
  store %struct.device* null, %struct.device** %2, align 8, !dbg !100
  br label %28, !dbg !100

; <label>:28:                                     ; preds = %27, %18
  %29 = load %struct.device*, %struct.device** %2, align 8, !dbg !101
  ret %struct.device* %29, !dbg !101
}

declare i32 @strcmp(i8*, i8*) #3

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !102 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.device*, align 8
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !105, metadata !38), !dbg !106
  %3 = load %struct.node*, %struct.node** @device_list_head, align 8, !dbg !107
  call void @init(%struct.node* %3), !dbg !108
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str.1, i32 0, i32 0)), !dbg !109
  call void @register_device(%struct.device* @dev1), !dbg !110
  call void @register_device(%struct.device* @dev2), !dbg !111
  %5 = call %struct.device* @lookup_device(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.2, i32 0, i32 0)), !dbg !112
  store %struct.device* %5, %struct.device** %2, align 8, !dbg !113
  %6 = load %struct.device*, %struct.device** %2, align 8, !dbg !114
  %7 = call i32 @open(%struct.device* %6), !dbg !115
  %8 = call %struct.device* @lookup_device(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.3, i32 0, i32 0)), !dbg !116
  store %struct.device* %8, %struct.device** %2, align 8, !dbg !117
  %9 = load %struct.device*, %struct.device** %2, align 8, !dbg !118
  %10 = call i32 @open(%struct.device* %9), !dbg !119
  ret i32 0, !dbg !120
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define internal void @dev1_open(%struct.device*) #0 !dbg !121 {
  %2 = alloca %struct.device*, align 8
  store %struct.device* %0, %struct.device** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !122, metadata !38), !dbg !123
  %3 = load %struct.device*, %struct.device** %2, align 8, !dbg !124
  %4 = getelementptr inbounds %struct.device, %struct.device* %3, i32 0, i32 2, !dbg !125
  store i32 0, i32* %4, align 8, !dbg !126
  ret void, !dbg !127
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define internal void @dev2_open(%struct.device*) #0 !dbg !128 {
  %2 = alloca %struct.device*, align 8
  store %struct.device* %0, %struct.device** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !129, metadata !38), !dbg !130
  %3 = load %struct.device*, %struct.device** %2, align 8, !dbg !131
  %4 = getelementptr inbounds %struct.device, %struct.device* %3, i32 0, i32 2, !dbg !132
  store i32 0, i32* %4, align 8, !dbg !133
  ret void, !dbg !134
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { allocsize(0) }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!29, !30, !31, !32}
!llvm.ident = !{!33}

!0 = !DIGlobalVariableExpression(var: !1)
!1 = distinct !DIGlobalVariable(name: "device_list_head", scope: !2, file: !3, line: 23, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !5, globals: !24)
!3 = !DIFile(filename: "kernel_example3.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/test_idl_gen")
!4 = !{}
!5 = !{!6, !23}
!6 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !7, size: 64)
!7 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "node", file: !3, line: 18, size: 128, elements: !8)
!8 = !{!9, !22}
!9 = !DIDerivedType(tag: DW_TAG_member, name: "dev", scope: !7, file: !3, line: 19, baseType: !10, size: 64)
!10 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!11 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "device", file: !3, line: 12, size: 192, elements: !12)
!12 = !{!13, !17, !20}
!13 = !DIDerivedType(tag: DW_TAG_member, name: "open", scope: !11, file: !3, line: 13, baseType: !14, size: 64)
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !15, size: 64)
!15 = !DISubroutineType(types: !16)
!16 = !{null, !10}
!17 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !11, file: !3, line: 14, baseType: !18, size: 64, offset: 64)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !19, size: 64)
!19 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!20 = !DIDerivedType(tag: DW_TAG_member, name: "devop_init_registered", scope: !11, file: !3, line: 15, baseType: !21, size: 32, offset: 128)
!21 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "next", scope: !7, file: !3, line: 20, baseType: !6, size: 64, offset: 64)
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!24 = !{!0, !25, !27}
!25 = !DIGlobalVariableExpression(var: !26)
!26 = distinct !DIGlobalVariable(name: "dev1", scope: !2, file: !3, line: 73, type: !11, isLocal: true, isDefinition: true)
!27 = !DIGlobalVariableExpression(var: !28)
!28 = distinct !DIGlobalVariable(name: "dev2", scope: !2, file: !3, line: 84, type: !11, isLocal: true, isDefinition: true)
!29 = !{i32 2, !"Dwarf Version", i32 4}
!30 = !{i32 2, !"Debug Info Version", i32 3}
!31 = !{i32 1, !"wchar_size", i32 4}
!32 = !{i32 7, !"PIC Level", i32 2}
!33 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!34 = distinct !DISubprogram(name: "init", scope: !3, file: !3, line: 27, type: !35, isLocal: false, isDefinition: true, scopeLine: 27, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!35 = !DISubroutineType(types: !36)
!36 = !{null, !6}
!37 = !DILocalVariable(name: "head", arg: 1, scope: !34, file: !3, line: 27, type: !6)
!38 = !DIExpression()
!39 = !DILocation(line: 27, column: 24, scope: !34)
!40 = !DILocation(line: 28, column: 10, scope: !34)
!41 = !DILocation(line: 29, column: 1, scope: !34)
!42 = distinct !DISubprogram(name: "register_device", scope: !3, file: !3, line: 31, type: !15, isLocal: false, isDefinition: true, scopeLine: 32, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!43 = !DILocalVariable(name: "dev", arg: 1, scope: !42, file: !3, line: 31, type: !10)
!44 = !DILocation(line: 31, column: 37, scope: !42)
!45 = !DILocalVariable(name: "new_node", scope: !42, file: !3, line: 34, type: !6)
!46 = !DILocation(line: 34, column: 18, scope: !42)
!47 = !DILocation(line: 34, column: 44, scope: !42)
!48 = !DILocation(line: 34, column: 29, scope: !42)
!49 = !DILocation(line: 37, column: 22, scope: !42)
!50 = !DILocation(line: 37, column: 5, scope: !42)
!51 = !DILocation(line: 37, column: 15, scope: !42)
!52 = !DILocation(line: 37, column: 20, scope: !42)
!53 = !DILocation(line: 40, column: 22, scope: !42)
!54 = !DILocation(line: 40, column: 5, scope: !42)
!55 = !DILocation(line: 40, column: 15, scope: !42)
!56 = !DILocation(line: 40, column: 20, scope: !42)
!57 = !DILocation(line: 43, column: 24, scope: !42)
!58 = !DILocation(line: 43, column: 22, scope: !42)
!59 = !DILocation(line: 44, column: 1, scope: !42)
!60 = distinct !DISubprogram(name: "open", scope: !3, file: !3, line: 46, type: !61, isLocal: false, isDefinition: true, scopeLine: 46, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!61 = !DISubroutineType(types: !62)
!62 = !{!21, !10}
!63 = !DILocalVariable(name: "dev", arg: 1, scope: !60, file: !3, line: 46, type: !10)
!64 = !DILocation(line: 46, column: 25, scope: !60)
!65 = !DILocation(line: 47, column: 2, scope: !60)
!66 = !DILocation(line: 47, column: 7, scope: !60)
!67 = !DILocation(line: 47, column: 12, scope: !60)
!68 = !DILocation(line: 48, column: 26, scope: !60)
!69 = !DILocation(line: 48, column: 31, scope: !60)
!70 = !DILocation(line: 48, column: 5, scope: !60)
!71 = !DILocation(line: 49, column: 5, scope: !60)
!72 = distinct !DISubprogram(name: "lookup_device", scope: !3, file: !3, line: 52, type: !73, isLocal: false, isDefinition: true, scopeLine: 53, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!73 = !DISubroutineType(types: !74)
!74 = !{!10, !18}
!75 = !DILocalVariable(name: "search_name", arg: 1, scope: !72, file: !3, line: 52, type: !18)
!76 = !DILocation(line: 52, column: 38, scope: !72)
!77 = !DILocalVariable(name: "current", scope: !72, file: !3, line: 54, type: !6)
!78 = !DILocation(line: 54, column: 18, scope: !72)
!79 = !DILocation(line: 54, column: 28, scope: !72)
!80 = !DILocation(line: 55, column: 5, scope: !72)
!81 = !DILocation(line: 55, column: 12, scope: !72)
!82 = !DILocation(line: 55, column: 20, scope: !72)
!83 = !DILocation(line: 57, column: 20, scope: !84)
!84 = distinct !DILexicalBlock(scope: !85, file: !3, line: 57, column: 13)
!85 = distinct !DILexicalBlock(scope: !72, file: !3, line: 56, column: 5)
!86 = !DILocation(line: 57, column: 29, scope: !84)
!87 = !DILocation(line: 57, column: 34, scope: !84)
!88 = !DILocation(line: 57, column: 39, scope: !84)
!89 = !DILocation(line: 57, column: 13, scope: !84)
!90 = !DILocation(line: 57, column: 52, scope: !84)
!91 = !DILocation(line: 57, column: 13, scope: !85)
!92 = !DILocation(line: 58, column: 20, scope: !84)
!93 = !DILocation(line: 58, column: 29, scope: !84)
!94 = !DILocation(line: 58, column: 13, scope: !84)
!95 = !DILocation(line: 60, column: 19, scope: !84)
!96 = !DILocation(line: 60, column: 28, scope: !84)
!97 = !DILocation(line: 60, column: 17, scope: !84)
!98 = distinct !{!98, !80, !99}
!99 = !DILocation(line: 61, column: 5, scope: !72)
!100 = !DILocation(line: 62, column: 5, scope: !72)
!101 = !DILocation(line: 64, column: 1, scope: !72)
!102 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 91, type: !103, isLocal: false, isDefinition: true, scopeLine: 91, isOptimized: false, unit: !2, variables: !4)
!103 = !DISubroutineType(types: !104)
!104 = !{!21}
!105 = !DILocalVariable(name: "dev", scope: !102, file: !3, line: 92, type: !10)
!106 = !DILocation(line: 92, column: 21, scope: !102)
!107 = !DILocation(line: 93, column: 10, scope: !102)
!108 = !DILocation(line: 93, column: 5, scope: !102)
!109 = !DILocation(line: 94, column: 5, scope: !102)
!110 = !DILocation(line: 95, column: 5, scope: !102)
!111 = !DILocation(line: 96, column: 5, scope: !102)
!112 = !DILocation(line: 98, column: 11, scope: !102)
!113 = !DILocation(line: 98, column: 9, scope: !102)
!114 = !DILocation(line: 99, column: 10, scope: !102)
!115 = !DILocation(line: 99, column: 5, scope: !102)
!116 = !DILocation(line: 100, column: 11, scope: !102)
!117 = !DILocation(line: 100, column: 9, scope: !102)
!118 = !DILocation(line: 101, column: 10, scope: !102)
!119 = !DILocation(line: 101, column: 5, scope: !102)
!120 = !DILocation(line: 103, column: 5, scope: !102)
!121 = distinct !DISubprogram(name: "dev1_open", scope: !3, file: !3, line: 69, type: !15, isLocal: true, isDefinition: true, scopeLine: 69, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!122 = !DILocalVariable(name: "dev", arg: 1, scope: !121, file: !3, line: 69, type: !10)
!123 = !DILocation(line: 69, column: 38, scope: !121)
!124 = !DILocation(line: 70, column: 2, scope: !121)
!125 = !DILocation(line: 70, column: 7, scope: !121)
!126 = !DILocation(line: 70, column: 29, scope: !121)
!127 = !DILocation(line: 71, column: 1, scope: !121)
!128 = distinct !DISubprogram(name: "dev2_open", scope: !3, file: !3, line: 80, type: !15, isLocal: true, isDefinition: true, scopeLine: 80, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!129 = !DILocalVariable(name: "dev", arg: 1, scope: !128, file: !3, line: 80, type: !10)
!130 = !DILocation(line: 80, column: 38, scope: !128)
!131 = !DILocation(line: 81, column: 2, scope: !128)
!132 = !DILocation(line: 81, column: 7, scope: !128)
!133 = !DILocation(line: 81, column: 29, scope: !128)
!134 = !DILocation(line: 82, column: 1, scope: !128)
