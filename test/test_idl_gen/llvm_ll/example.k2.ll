; ModuleID = 'example.k2.c'
source_filename = "example.k2.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.node = type { %struct.device*, %struct.node* }
%struct.device = type { void (%struct.device*)*, i8*, i32 }

@device_list = common global %struct.node* null, align 8, !dbg !0
@.str = private unnamed_addr constant [25 x i8] c"Add device to the list:\0A\00", align 1
@dev1 = internal global %struct.device { void (%struct.device*)* @dev1_open, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.1, i32 0, i32 0), i32 0 }, align 8, !dbg !25
@.str.1 = private unnamed_addr constant [9 x i8] c"device_1\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define void @init(%struct.node* %head) #0 !dbg !31 {
entry:
  %head.addr = alloca %struct.node*, align 8
  store %struct.node* %head, %struct.node** %head.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.node** %head.addr, metadata !34, metadata !DIExpression()), !dbg !35
  store %struct.node* null, %struct.node** %head.addr, align 8, !dbg !36
  ret void, !dbg !37
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define void @register_device(%struct.device* %device_option) #0 !dbg !38 {
entry:
  %device_option.addr = alloca %struct.device*, align 8
  %temp = alloca %struct.node*, align 8
  store %struct.device* %device_option, %struct.device** %device_option.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %device_option.addr, metadata !39, metadata !DIExpression()), !dbg !40
  call void @llvm.dbg.declare(metadata %struct.node** %temp, metadata !41, metadata !DIExpression()), !dbg !42
  %call = call noalias i8* @malloc(i64 16) #5, !dbg !43
  %0 = bitcast i8* %call to %struct.node*, !dbg !44
  store %struct.node* %0, %struct.node** %temp, align 8, !dbg !42
  %1 = load %struct.node*, %struct.node** %temp, align 8, !dbg !45
  %cmp = icmp eq %struct.node* %1, null, !dbg !47
  br i1 %cmp, label %if.then, label %if.end, !dbg !48

if.then:                                          ; preds = %entry
  call void @exit(i32 0) #6, !dbg !49
  unreachable, !dbg !49

if.end:                                           ; preds = %entry
  %2 = load %struct.device*, %struct.device** %device_option.addr, align 8, !dbg !51
  %3 = load %struct.node*, %struct.node** %temp, align 8, !dbg !52
  %dev = getelementptr inbounds %struct.node, %struct.node* %3, i32 0, i32 0, !dbg !53
  store %struct.device* %2, %struct.device** %dev, align 8, !dbg !54
  %4 = load %struct.node*, %struct.node** @device_list, align 8, !dbg !55
  %5 = load %struct.node*, %struct.node** %temp, align 8, !dbg !56
  %next = getelementptr inbounds %struct.node, %struct.node* %5, i32 0, i32 1, !dbg !57
  store %struct.node* %4, %struct.node** %next, align 8, !dbg !58
  %6 = load %struct.node*, %struct.node** %temp, align 8, !dbg !59
  store %struct.node* %6, %struct.node** @device_list, align 8, !dbg !60
  ret void, !dbg !61
}

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #2

; Function Attrs: noreturn nounwind
declare void @exit(i32) #3

; Function Attrs: noinline nounwind optnone uwtable
define i32 @open(%struct.device* %dev) #0 !dbg !62 {
entry:
  %dev.addr = alloca %struct.device*, align 8
  store %struct.device* %dev, %struct.device** %dev.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %dev.addr, metadata !65, metadata !DIExpression()), !dbg !66
  %0 = load %struct.device*, %struct.device** %dev.addr, align 8, !dbg !67
  %open = getelementptr inbounds %struct.device, %struct.device* %0, i32 0, i32 0, !dbg !68
  %1 = load void (%struct.device*)*, void (%struct.device*)** %open, align 8, !dbg !68
  %2 = load %struct.device*, %struct.device** %dev.addr, align 8, !dbg !69
  call void %1(%struct.device* %2), !dbg !67
  ret i32 1, !dbg !70
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 !dbg !71 {
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %0 = load %struct.node*, %struct.node** @device_list, align 8, !dbg !74
  call void @init(%struct.node* %0), !dbg !75
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str, i32 0, i32 0)), !dbg !76
  call void @register_device(%struct.device* @dev1), !dbg !77
  %call1 = call i32 @open(%struct.device* @dev1), !dbg !78
  ret i32 0, !dbg !79
}

declare i32 @printf(i8*, ...) #4

; Function Attrs: noinline nounwind optnone uwtable
define internal void @dev1_open(%struct.device* %dev) #0 !dbg !80 {
entry:
  %dev.addr = alloca %struct.device*, align 8
  store %struct.device* %dev, %struct.device** %dev.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.device** %dev.addr, metadata !81, metadata !DIExpression()), !dbg !82
  %0 = load %struct.device*, %struct.device** %dev.addr, align 8, !dbg !83
  %devop_init_registered = getelementptr inbounds %struct.device, %struct.device* %0, i32 0, i32 2, !dbg !84
  store i32 0, i32* %devop_init_registered, align 8, !dbg !85
  ret void, !dbg !86
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind }
attributes #6 = { noreturn nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!27, !28, !29}
!llvm.ident = !{!30}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "device_list", scope: !2, file: !3, line: 20, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !5, globals: !24)
!3 = !DIFile(filename: "example.k2.c", directory: "/home/yongzhe/llvm-versions/llvm-5.0/lib/Analysis/PDG/test/test_idl_gen")
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
!25 = !DIGlobalVariableExpression(var: !26, expr: !DIExpression())
!26 = distinct !DIGlobalVariable(name: "dev1", scope: !2, file: !3, line: 55, type: !11, isLocal: true, isDefinition: true)
!27 = !{i32 2, !"Dwarf Version", i32 4}
!28 = !{i32 2, !"Debug Info Version", i32 3}
!29 = !{i32 1, !"wchar_size", i32 4}
!30 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
!31 = distinct !DISubprogram(name: "init", scope: !3, file: !3, line: 24, type: !32, isLocal: false, isDefinition: true, scopeLine: 24, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!32 = !DISubroutineType(types: !33)
!33 = !{null, !6}
!34 = !DILocalVariable(name: "head", arg: 1, scope: !31, file: !3, line: 24, type: !6)
!35 = !DILocation(line: 24, column: 24, scope: !31)
!36 = !DILocation(line: 25, column: 10, scope: !31)
!37 = !DILocation(line: 26, column: 1, scope: !31)
!38 = distinct !DISubprogram(name: "register_device", scope: !3, file: !3, line: 28, type: !15, isLocal: false, isDefinition: true, scopeLine: 28, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!39 = !DILocalVariable(name: "device_option", arg: 1, scope: !38, file: !3, line: 28, type: !10)
!40 = !DILocation(line: 28, column: 37, scope: !38)
!41 = !DILocalVariable(name: "temp", scope: !38, file: !3, line: 29, type: !6)
!42 = !DILocation(line: 29, column: 18, scope: !38)
!43 = !DILocation(line: 29, column: 40, scope: !38)
!44 = !DILocation(line: 29, column: 25, scope: !38)
!45 = !DILocation(line: 30, column: 9, scope: !46)
!46 = distinct !DILexicalBlock(scope: !38, file: !3, line: 30, column: 9)
!47 = !DILocation(line: 30, column: 14, scope: !46)
!48 = !DILocation(line: 30, column: 9, scope: !38)
!49 = !DILocation(line: 31, column: 9, scope: !50)
!50 = distinct !DILexicalBlock(scope: !46, file: !3, line: 30, column: 23)
!51 = !DILocation(line: 35, column: 17, scope: !38)
!52 = !DILocation(line: 35, column: 5, scope: !38)
!53 = !DILocation(line: 35, column: 11, scope: !38)
!54 = !DILocation(line: 35, column: 15, scope: !38)
!55 = !DILocation(line: 38, column: 18, scope: !38)
!56 = !DILocation(line: 38, column: 5, scope: !38)
!57 = !DILocation(line: 38, column: 11, scope: !38)
!58 = !DILocation(line: 38, column: 16, scope: !38)
!59 = !DILocation(line: 40, column: 19, scope: !38)
!60 = !DILocation(line: 40, column: 17, scope: !38)
!61 = !DILocation(line: 41, column: 1, scope: !38)
!62 = distinct !DISubprogram(name: "open", scope: !3, file: !3, line: 43, type: !63, isLocal: false, isDefinition: true, scopeLine: 43, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!63 = !DISubroutineType(types: !64)
!64 = !{!21, !10}
!65 = !DILocalVariable(name: "dev", arg: 1, scope: !62, file: !3, line: 43, type: !10)
!66 = !DILocation(line: 43, column: 25, scope: !62)
!67 = !DILocation(line: 44, column: 2, scope: !62)
!68 = !DILocation(line: 44, column: 7, scope: !62)
!69 = !DILocation(line: 44, column: 12, scope: !62)
!70 = !DILocation(line: 45, column: 5, scope: !62)
!71 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 62, type: !72, isLocal: false, isDefinition: true, scopeLine: 62, isOptimized: false, unit: !2, variables: !4)
!72 = !DISubroutineType(types: !73)
!73 = !{!21}
!74 = !DILocation(line: 63, column: 10, scope: !71)
!75 = !DILocation(line: 63, column: 5, scope: !71)
!76 = !DILocation(line: 64, column: 5, scope: !71)
!77 = !DILocation(line: 65, column: 5, scope: !71)
!78 = !DILocation(line: 66, column: 5, scope: !71)
!79 = !DILocation(line: 67, column: 5, scope: !71)
!80 = distinct !DISubprogram(name: "dev1_open", scope: !3, file: !3, line: 51, type: !15, isLocal: true, isDefinition: true, scopeLine: 51, flags: DIFlagPrototyped, isOptimized: false, unit: !2, variables: !4)
!81 = !DILocalVariable(name: "dev", arg: 1, scope: !80, file: !3, line: 51, type: !10)
!82 = !DILocation(line: 51, column: 38, scope: !80)
!83 = !DILocation(line: 52, column: 2, scope: !80)
!84 = !DILocation(line: 52, column: 7, scope: !80)
!85 = !DILocation(line: 52, column: 29, scope: !80)
!86 = !DILocation(line: 53, column: 1, scope: !80)
