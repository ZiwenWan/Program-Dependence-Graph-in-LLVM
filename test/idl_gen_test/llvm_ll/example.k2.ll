; ModuleID = 'example.k2.c'
source_filename = "example.k2.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.node = type { %struct.device*, %struct.node* }
%struct.device = type { void (%struct.device*)*, i8*, i32 }

@device_list = common global %struct.node* null, align 8, !dbg !0
@.str = private unnamed_addr constant [25 x i8] c"Add device to the list:\0A\00", align 1
@dev1 = internal global %struct.device { void (%struct.device*)* @dev1_open, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.1, i32 0, i32 0), i32 0 }, align 8, !dbg !25
@.str.1 = private unnamed_addr constant [9 x i8] c"device_1\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @init(%struct.node*) #0 !dbg !32 {
  %2 = alloca %struct.node*, align 8
  store %struct.node* %0, %struct.node** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.node** %2, metadata !35, metadata !36), !dbg !37
  store %struct.node* null, %struct.node** %2, align 8, !dbg !38
  ret void, !dbg !39
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @register_device(%struct.device*) #0 !dbg !40 {
  %2 = alloca %struct.device*, align 8
  %3 = alloca %struct.node*, align 8
  store %struct.device* %0, %struct.device** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !41, metadata !36), !dbg !42
  call void @llvm.dbg.declare(metadata %struct.node** %3, metadata !43, metadata !36), !dbg !44
  %4 = call i8* @malloc(i64 16) #5, !dbg !45
  %5 = bitcast i8* %4 to %struct.node*, !dbg !46
  store %struct.node* %5, %struct.node** %3, align 8, !dbg !44
  %6 = load %struct.node*, %struct.node** %3, align 8, !dbg !47
  %7 = icmp eq %struct.node* %6, null, !dbg !49
  br i1 %7, label %8, label %9, !dbg !50

; <label>:8:                                      ; preds = %1
  call void @exit(i32 0) #6, !dbg !51
  unreachable, !dbg !51

; <label>:9:                                      ; preds = %1
  %10 = load %struct.device*, %struct.device** %2, align 8, !dbg !53
  %11 = load %struct.node*, %struct.node** %3, align 8, !dbg !54
  %12 = getelementptr inbounds %struct.node, %struct.node* %11, i32 0, i32 0, !dbg !55
  store %struct.device* %10, %struct.device** %12, align 8, !dbg !56
  %13 = load %struct.node*, %struct.node** @device_list, align 8, !dbg !57
  %14 = load %struct.node*, %struct.node** %3, align 8, !dbg !58
  %15 = getelementptr inbounds %struct.node, %struct.node* %14, i32 0, i32 1, !dbg !59
  store %struct.node* %13, %struct.node** %15, align 8, !dbg !60
  %16 = load %struct.node*, %struct.node** %3, align 8, !dbg !61
  store %struct.node* %16, %struct.node** @device_list, align 8, !dbg !62
  ret void, !dbg !63
}

; Function Attrs: allocsize(0)
declare i8* @malloc(i64) #2

; Function Attrs: noreturn
declare void @exit(i32) #3

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @open(%struct.device*) #0 !dbg !64 {
  %2 = alloca %struct.device*, align 8
  store %struct.device* %0, %struct.device** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !67, metadata !36), !dbg !68
  %3 = load %struct.device*, %struct.device** %2, align 8, !dbg !69
  %4 = getelementptr inbounds %struct.device, %struct.device* %3, i32 0, i32 0, !dbg !70
  %5 = load void (%struct.device*)*, void (%struct.device*)** %4, align 8, !dbg !70
  %6 = load %struct.device*, %struct.device** %2, align 8, !dbg !71
  call void %5(%struct.device* %6), !dbg !69
  ret i32 1, !dbg !72
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !73 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = load %struct.node*, %struct.node** @device_list, align 8, !dbg !76
  call void @init(%struct.node* %2), !dbg !77
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str, i32 0, i32 0)), !dbg !78
  call void @register_device(%struct.device* @dev1), !dbg !79
  %4 = call i32 @open(%struct.device* @dev1), !dbg !80
  ret i32 0, !dbg !81
}

declare i32 @printf(i8*, ...) #4

; Function Attrs: noinline nounwind optnone ssp uwtable
define internal void @dev1_open(%struct.device*) #0 !dbg !82 {
  %2 = alloca %struct.device*, align 8
  store %struct.device* %0, %struct.device** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %2, metadata !83, metadata !36), !dbg !84
  %3 = load %struct.device*, %struct.device** %2, align 8, !dbg !85
  %4 = getelementptr inbounds %struct.device, %struct.device* %3, i32 0, i32 2, !dbg !86
  store i32 0, i32* %4, align 8, !dbg !87
  ret void, !dbg !88
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { allocsize(0) }
attributes #6 = { noreturn }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!27, !28, !29, !30}
!llvm.ident = !{!31}

!0 = !DIGlobalVariableExpression(var: !1)
!1 = distinct !DIGlobalVariable(name: "device_list", scope: !2, file: !3, line: 20, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !5, globals: !24)
!3 = !DIFile(filename: "example.k2.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/idl_gen_test")
!4 = !{}
!5 = !{!6, !23}
!6 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !7, size: 64)
!7 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "node", file: !3, line: 15, size: 128, elements: !8)
!8 = !{!9, !22}
!9 = !DIDerivedType(tag: DW_TAG_member, name: "dev", scope: !7, file: !3, line: 16, baseType: !10, size: 64)
!10 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!11 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "device", file: !3, line: 9, size: 192, elements: !12)
!12 = !{!13, !17, !20}
!13 = !DIDerivedType(tag: DW_TAG_member, name: "open", scope: !11, file: !3, line: 10, baseType: !14, size: 64)
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !15, size: 64)
!15 = !DISubroutineType(types: !16)
!16 = !{null, !10}
!17 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !11, file: !3, line: 11, baseType: !18, size: 64, offset: 64)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !19, size: 64)
!19 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!20 = !DIDerivedType(tag: DW_TAG_member, name: "devop_init_registered", scope: !11, file: !3, line: 12, baseType: !21, size: 32, offset: 128)
!21 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "next", scope: !7, file: !3, line: 17, baseType: !6, size: 64, offset: 64)
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!24 = !{!0, !25}
!25 = !DIGlobalVariableExpression(var: !26)
!26 = distinct !DIGlobalVariable(name: "dev1", scope: !2, file: !3, line: 55, type: !11, isLocal: true, isDefinition: true)
!27 = !{i32 2, !"Dwarf Version", i32 4}
!28 = !{i32 2, !"Debug Info Version", i32 3}
!29 = !{i32 1, !"wchar_size", i32 4}
!30 = !{i32 7, !"PIC Level", i32 2}
!31 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!32 = distinct !DISubprogram(name: "init", scope: !3, file: !3, line: 24, type: !33, isLocal: false, isDefinition: true, scopeLine: 24, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!33 = !DISubroutineType(types: !34)
!34 = !{null, !6}
!35 = !DILocalVariable(name: "head", arg: 1, scope: !32, file: !3, line: 24, type: !6)
!36 = !DIExpression()
!37 = !DILocation(line: 24, column: 24, scope: !32)
!38 = !DILocation(line: 25, column: 10, scope: !32)
!39 = !DILocation(line: 26, column: 1, scope: !32)
!40 = distinct !DISubprogram(name: "register_device", scope: !3, file: !3, line: 28, type: !15, isLocal: false, isDefinition: true, scopeLine: 28, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!41 = !DILocalVariable(name: "device_option", arg: 1, scope: !40, file: !3, line: 28, type: !10)
!42 = !DILocation(line: 28, column: 37, scope: !40)
!43 = !DILocalVariable(name: "temp", scope: !40, file: !3, line: 29, type: !6)
!44 = !DILocation(line: 29, column: 18, scope: !40)
!45 = !DILocation(line: 29, column: 40, scope: !40)
!46 = !DILocation(line: 29, column: 25, scope: !40)
!47 = !DILocation(line: 30, column: 9, scope: !48)
!48 = distinct !DILexicalBlock(scope: !40, file: !3, line: 30, column: 9)
!49 = !DILocation(line: 30, column: 14, scope: !48)
!50 = !DILocation(line: 30, column: 9, scope: !40)
!51 = !DILocation(line: 31, column: 9, scope: !52)
!52 = distinct !DILexicalBlock(scope: !48, file: !3, line: 30, column: 23)
!53 = !DILocation(line: 35, column: 17, scope: !40)
!54 = !DILocation(line: 35, column: 5, scope: !40)
!55 = !DILocation(line: 35, column: 11, scope: !40)
!56 = !DILocation(line: 35, column: 15, scope: !40)
!57 = !DILocation(line: 38, column: 18, scope: !40)
!58 = !DILocation(line: 38, column: 5, scope: !40)
!59 = !DILocation(line: 38, column: 11, scope: !40)
!60 = !DILocation(line: 38, column: 16, scope: !40)
!61 = !DILocation(line: 40, column: 19, scope: !40)
!62 = !DILocation(line: 40, column: 17, scope: !40)
!63 = !DILocation(line: 41, column: 1, scope: !40)
!64 = distinct !DISubprogram(name: "open", scope: !3, file: !3, line: 43, type: !65, isLocal: false, isDefinition: true, scopeLine: 43, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!65 = !DISubroutineType(types: !66)
!66 = !{!21, !10}
!67 = !DILocalVariable(name: "dev", arg: 1, scope: !64, file: !3, line: 43, type: !10)
!68 = !DILocation(line: 43, column: 25, scope: !64)
!69 = !DILocation(line: 44, column: 2, scope: !64)
!70 = !DILocation(line: 44, column: 7, scope: !64)
!71 = !DILocation(line: 44, column: 12, scope: !64)
!72 = !DILocation(line: 45, column: 5, scope: !64)
!73 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 62, type: !74, isLocal: false, isDefinition: true, scopeLine: 62, isOptimized: false, unit: !2, variables: !4)
!74 = !DISubroutineType(types: !75)
!75 = !{!21}
!76 = !DILocation(line: 63, column: 10, scope: !73)
!77 = !DILocation(line: 63, column: 5, scope: !73)
!78 = !DILocation(line: 64, column: 5, scope: !73)
!79 = !DILocation(line: 65, column: 5, scope: !73)
!80 = !DILocation(line: 66, column: 5, scope: !73)
!81 = !DILocation(line: 67, column: 5, scope: !73)
!82 = distinct !DISubprogram(name: "dev1_open", scope: !3, file: !3, line: 51, type: !15, isLocal: true, isDefinition: true, scopeLine: 51, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!83 = !DILocalVariable(name: "dev", arg: 1, scope: !82, file: !3, line: 51, type: !10)
!84 = !DILocation(line: 51, column: 38, scope: !82)
!85 = !DILocation(line: 52, column: 2, scope: !82)
!86 = !DILocation(line: 52, column: 7, scope: !82)
!87 = !DILocation(line: 52, column: 29, scope: !82)
!88 = !DILocation(line: 53, column: 1, scope: !82)
