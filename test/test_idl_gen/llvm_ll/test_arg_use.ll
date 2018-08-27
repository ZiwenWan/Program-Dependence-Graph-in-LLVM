; ModuleID = 'test_arg_use.c'
source_filename = "test_arg_use.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.clothes = type { [10 x i8], float }
%struct.person = type { i32, [10 x i8], %struct.clothes* }

@.str = private unnamed_addr constant [23 x i8] c"Age is %d, name is %s\0A\00", align 1
@.str.1 = private unnamed_addr constant [19 x i8] c"Changed age is %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [15 x i8] c"Now age is %d\0A\00", align 1
@main.c = private unnamed_addr constant %struct.clothes { [10 x i8] c"red\00\00\00\00\00\00\00", float 0x400B333340000000 }, align 4
@.str.3 = private unnamed_addr constant [10 x i8] c"Jack\00\00\00\00\00\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define i32 @getAge(%struct.person* %p) #0 !dbg !7 {
entry:
  %p.addr = alloca %struct.person*, align 8
  store %struct.person* %p, %struct.person** %p.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.person** %p.addr, metadata !29, metadata !DIExpression()), !dbg !30
  %0 = load %struct.person*, %struct.person** %p.addr, align 8, !dbg !31
  %age = getelementptr inbounds %struct.person, %struct.person* %0, i32 0, i32 0, !dbg !32
  %1 = load i32, i32* %age, align 8, !dbg !32
  ret i32 %1, !dbg !33
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define void @changeAge(%struct.person* %p) #0 !dbg !34 {
entry:
  %p.addr = alloca %struct.person*, align 8
  store %struct.person* %p, %struct.person** %p.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.person** %p.addr, metadata !37, metadata !DIExpression()), !dbg !38
  %0 = load %struct.person*, %struct.person** %p.addr, align 8, !dbg !39
  %age = getelementptr inbounds %struct.person, %struct.person* %0, i32 0, i32 0, !dbg !40
  store i32 50, i32* %age, align 8, !dbg !41
  ret void, !dbg !42
}

; Function Attrs: noinline nounwind optnone uwtable
define void @printAge(%struct.person* %p) #0 !dbg !43 {
entry:
  %p.addr = alloca %struct.person*, align 8
  %age = alloca i32, align 4
  %name = alloca i8*, align 8
  store %struct.person* %p, %struct.person** %p.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.person** %p.addr, metadata !44, metadata !DIExpression()), !dbg !45
  call void @llvm.dbg.declare(metadata i32* %age, metadata !46, metadata !DIExpression()), !dbg !47
  %0 = load %struct.person*, %struct.person** %p.addr, align 8, !dbg !48
  %call = call i32 @getAge(%struct.person* %0), !dbg !49
  store i32 %call, i32* %age, align 4, !dbg !47
  call void @llvm.dbg.declare(metadata i8** %name, metadata !50, metadata !DIExpression()), !dbg !52
  %1 = load %struct.person*, %struct.person** %p.addr, align 8, !dbg !53
  %name1 = getelementptr inbounds %struct.person, %struct.person* %1, i32 0, i32 1, !dbg !54
  %arraydecay = getelementptr inbounds [10 x i8], [10 x i8]* %name1, i32 0, i32 0, !dbg !53
  store i8* %arraydecay, i8** %name, align 8, !dbg !52
  %2 = load i32, i32* %age, align 4, !dbg !55
  %3 = load i8*, i8** %name, align 8, !dbg !56
  %call2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str, i32 0, i32 0), i32 %2, i8* %3), !dbg !57
  %4 = load %struct.person*, %struct.person** %p.addr, align 8, !dbg !58
  call void @changeAge(%struct.person* %4), !dbg !59
  %5 = load %struct.person*, %struct.person** %p.addr, align 8, !dbg !60
  %age3 = getelementptr inbounds %struct.person, %struct.person* %5, i32 0, i32 0, !dbg !61
  %6 = load i32, i32* %age3, align 8, !dbg !61
  %call4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.1, i32 0, i32 0), i32 %6), !dbg !62
  ret void, !dbg !63
}

declare i32 @printf(i8*, ...) #2

; Function Attrs: noinline nounwind optnone uwtable
define void @changeAgePtr(i32* %age) #0 !dbg !64 {
entry:
  %age.addr = alloca i32*, align 8
  store i32* %age, i32** %age.addr, align 8
  call void @llvm.dbg.declare(metadata i32** %age.addr, metadata !68, metadata !DIExpression()), !dbg !69
  %0 = load i32*, i32** %age.addr, align 8, !dbg !70
  store i32 100, i32* %0, align 4, !dbg !71
  ret void, !dbg !72
}

; Function Attrs: noinline nounwind optnone uwtable
define void @agePtr(%struct.person* %p) #0 !dbg !73 {
entry:
  %p.addr = alloca %struct.person*, align 8
  %age = alloca i32*, align 8
  store %struct.person* %p, %struct.person** %p.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.person** %p.addr, metadata !74, metadata !DIExpression()), !dbg !75
  call void @llvm.dbg.declare(metadata i32** %age, metadata !76, metadata !DIExpression()), !dbg !77
  %0 = load %struct.person*, %struct.person** %p.addr, align 8, !dbg !78
  %age1 = getelementptr inbounds %struct.person, %struct.person* %0, i32 0, i32 0, !dbg !79
  store i32* %age1, i32** %age, align 8, !dbg !77
  %1 = load i32*, i32** %age, align 8, !dbg !80
  call void @changeAgePtr(i32* %1), !dbg !81
  %2 = load i32*, i32** %age, align 8, !dbg !82
  %3 = load i32, i32* %2, align 4, !dbg !83
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([15 x i8], [15 x i8]* @.str.2, i32 0, i32 0), i32 %3), !dbg !84
  ret void, !dbg !85
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 !dbg !86 {
entry:
  %retval = alloca i32, align 4
  %c = alloca %struct.clothes, align 4
  %p = alloca %struct.person, align 8
  store i32 0, i32* %retval, align 4
  call void @llvm.dbg.declare(metadata %struct.clothes* %c, metadata !89, metadata !DIExpression()), !dbg !90
  %0 = bitcast %struct.clothes* %c to i8*, !dbg !90
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %0, i8* align 4 getelementptr inbounds (%struct.clothes, %struct.clothes* @main.c, i32 0, i32 0, i32 0), i64 16, i1 false), !dbg !90
  call void @llvm.dbg.declare(metadata %struct.person* %p, metadata !91, metadata !DIExpression()), !dbg !92
  %age = getelementptr inbounds %struct.person, %struct.person* %p, i32 0, i32 0, !dbg !93
  store i32 20, i32* %age, align 8, !dbg !93
  %name = getelementptr inbounds %struct.person, %struct.person* %p, i32 0, i32 1, !dbg !93
  %1 = bitcast [10 x i8]* %name to i8*, !dbg !94
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %1, i8* align 1 getelementptr inbounds ([10 x i8], [10 x i8]* @.str.3, i32 0, i32 0), i64 10, i1 false), !dbg !94
  %clothes = getelementptr inbounds %struct.person, %struct.person* %p, i32 0, i32 2, !dbg !93
  store %struct.clothes* %c, %struct.clothes** %clothes, align 8, !dbg !93
  call void @printAge(%struct.person* %p), !dbg !95
  ret i32 0, !dbg !96
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i1) #3

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { argmemonly nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5}
!llvm.ident = !{!6}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test_arg_use.c", directory: "/home/yongzhe/llvm-versions/llvm-5.0/lib/Analysis/CDG/test/test_idl_gen")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
!7 = distinct !DISubprogram(name: "getAge", scope: !1, file: !1, line: 16, type: !8, isLocal: false, isDefinition: true, scopeLine: 16, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!8 = !DISubroutineType(types: !9)
!9 = !{!10, !11}
!10 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DIDerivedType(tag: DW_TAG_typedef, name: "Person", file: !1, line: 13, baseType: !13)
!13 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "person", file: !1, line: 9, size: 192, elements: !14)
!14 = !{!15, !16, !21}
!15 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !13, file: !1, line: 10, baseType: !10, size: 32)
!16 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !13, file: !1, line: 11, baseType: !17, size: 80, offset: 32)
!17 = !DICompositeType(tag: DW_TAG_array_type, baseType: !18, size: 80, elements: !19)
!18 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!19 = !{!20}
!20 = !DISubrange(count: 10)
!21 = !DIDerivedType(tag: DW_TAG_member, name: "clothes", scope: !13, file: !1, line: 12, baseType: !22, size: 64, offset: 128)
!22 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !23, size: 64)
!23 = !DIDerivedType(tag: DW_TAG_typedef, name: "Clothes", file: !1, line: 7, baseType: !24)
!24 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "clothes", file: !1, line: 4, size: 128, elements: !25)
!25 = !{!26, !27}
!26 = !DIDerivedType(tag: DW_TAG_member, name: "color", scope: !24, file: !1, line: 5, baseType: !17, size: 80)
!27 = !DIDerivedType(tag: DW_TAG_member, name: "height", scope: !24, file: !1, line: 6, baseType: !28, size: 32, offset: 96)
!28 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!29 = !DILocalVariable(name: "p", arg: 1, scope: !7, file: !1, line: 16, type: !11)
!30 = !DILocation(line: 16, column: 20, scope: !7)
!31 = !DILocation(line: 17, column: 12, scope: !7)
!32 = !DILocation(line: 17, column: 15, scope: !7)
!33 = !DILocation(line: 17, column: 5, scope: !7)
!34 = distinct !DISubprogram(name: "changeAge", scope: !1, file: !1, line: 20, type: !35, isLocal: false, isDefinition: true, scopeLine: 20, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!35 = !DISubroutineType(types: !36)
!36 = !{null, !11}
!37 = !DILocalVariable(name: "p", arg: 1, scope: !34, file: !1, line: 20, type: !11)
!38 = !DILocation(line: 20, column: 24, scope: !34)
!39 = !DILocation(line: 21, column: 5, scope: !34)
!40 = !DILocation(line: 21, column: 8, scope: !34)
!41 = !DILocation(line: 21, column: 12, scope: !34)
!42 = !DILocation(line: 22, column: 1, scope: !34)
!43 = distinct !DISubprogram(name: "printAge", scope: !1, file: !1, line: 24, type: !35, isLocal: false, isDefinition: true, scopeLine: 24, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!44 = !DILocalVariable(name: "p", arg: 1, scope: !43, file: !1, line: 24, type: !11)
!45 = !DILocation(line: 24, column: 23, scope: !43)
!46 = !DILocalVariable(name: "age", scope: !43, file: !1, line: 25, type: !10)
!47 = !DILocation(line: 25, column: 9, scope: !43)
!48 = !DILocation(line: 25, column: 22, scope: !43)
!49 = !DILocation(line: 25, column: 15, scope: !43)
!50 = !DILocalVariable(name: "name", scope: !43, file: !1, line: 26, type: !51)
!51 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !18, size: 64)
!52 = !DILocation(line: 26, column: 11, scope: !43)
!53 = !DILocation(line: 26, column: 18, scope: !43)
!54 = !DILocation(line: 26, column: 21, scope: !43)
!55 = !DILocation(line: 27, column: 39, scope: !43)
!56 = !DILocation(line: 27, column: 44, scope: !43)
!57 = !DILocation(line: 27, column: 5, scope: !43)
!58 = !DILocation(line: 28, column: 15, scope: !43)
!59 = !DILocation(line: 28, column: 5, scope: !43)
!60 = !DILocation(line: 29, column: 35, scope: !43)
!61 = !DILocation(line: 29, column: 38, scope: !43)
!62 = !DILocation(line: 29, column: 5, scope: !43)
!63 = !DILocation(line: 30, column: 1, scope: !43)
!64 = distinct !DISubprogram(name: "changeAgePtr", scope: !1, file: !1, line: 33, type: !65, isLocal: false, isDefinition: true, scopeLine: 33, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!65 = !DISubroutineType(types: !66)
!66 = !{null, !67}
!67 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !10, size: 64)
!68 = !DILocalVariable(name: "age", arg: 1, scope: !64, file: !1, line: 33, type: !67)
!69 = !DILocation(line: 33, column: 24, scope: !64)
!70 = !DILocation(line: 34, column: 5, scope: !64)
!71 = !DILocation(line: 34, column: 9, scope: !64)
!72 = !DILocation(line: 35, column: 1, scope: !64)
!73 = distinct !DISubprogram(name: "agePtr", scope: !1, file: !1, line: 37, type: !35, isLocal: false, isDefinition: true, scopeLine: 37, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!74 = !DILocalVariable(name: "p", arg: 1, scope: !73, file: !1, line: 37, type: !11)
!75 = !DILocation(line: 37, column: 21, scope: !73)
!76 = !DILocalVariable(name: "age", scope: !73, file: !1, line: 38, type: !67)
!77 = !DILocation(line: 38, column: 10, scope: !73)
!78 = !DILocation(line: 38, column: 18, scope: !73)
!79 = !DILocation(line: 38, column: 21, scope: !73)
!80 = !DILocation(line: 39, column: 18, scope: !73)
!81 = !DILocation(line: 39, column: 5, scope: !73)
!82 = !DILocation(line: 40, column: 32, scope: !73)
!83 = !DILocation(line: 40, column: 31, scope: !73)
!84 = !DILocation(line: 40, column: 5, scope: !73)
!85 = !DILocation(line: 41, column: 1, scope: !73)
!86 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 44, type: !87, isLocal: false, isDefinition: true, scopeLine: 44, isOptimized: false, unit: !0, variables: !2)
!87 = !DISubroutineType(types: !88)
!88 = !{!10}
!89 = !DILocalVariable(name: "c", scope: !86, file: !1, line: 45, type: !23)
!90 = !DILocation(line: 45, column: 13, scope: !86)
!91 = !DILocalVariable(name: "p", scope: !86, file: !1, line: 46, type: !12)
!92 = !DILocation(line: 46, column: 12, scope: !86)
!93 = !DILocation(line: 46, column: 16, scope: !86)
!94 = !DILocation(line: 46, column: 22, scope: !86)
!95 = !DILocation(line: 48, column: 5, scope: !86)
!96 = !DILocation(line: 49, column: 5, scope: !86)
