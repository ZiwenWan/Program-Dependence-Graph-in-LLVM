; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Person = type { i32, [10 x i8] }

@.str = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"new_name\00", align 1
@.str.2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@main.p = private unnamed_addr constant %struct.Person { i32 15, [10 x i8] c"Jack\00\00\00\00\00\00" }, align 4

; Function Attrs: noinline nounwind optnone uwtable
define void @func1(i8* %name) #0 !dbg !7 {
entry:
  %name.addr = alloca i8*, align 8
  store i8* %name, i8** %name.addr, align 8
  call void @llvm.dbg.declare(metadata i8** %name.addr, metadata !12, metadata !DIExpression()), !dbg !13
  %0 = load i8*, i8** %name.addr, align 8, !dbg !14
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i8* %0), !dbg !15
  %1 = load i8*, i8** %name.addr, align 8, !dbg !16
  %call1 = call i8* @strncpy(i8* %1, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.1, i32 0, i32 0), i64 10) #5, !dbg !17
  ret void, !dbg !18
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @printf(i8*, ...) #2

; Function Attrs: nounwind
declare i8* @strncpy(i8*, i8*, i64) #3

; Function Attrs: noinline nounwind optnone uwtable
define void @func2(i32* %age) #0 !dbg !19 {
entry:
  %age.addr = alloca i32*, align 8
  store i32* %age, i32** %age.addr, align 8
  call void @llvm.dbg.declare(metadata i32** %age.addr, metadata !24, metadata !DIExpression()), !dbg !25
  %0 = load i32*, i32** %age.addr, align 8, !dbg !26
  %add.ptr = getelementptr inbounds i32, i32* %0, i64 1, !dbg !26
  store i32* %add.ptr, i32** %age.addr, align 8, !dbg !26
  ret void, !dbg !27
}

; Function Attrs: noinline nounwind optnone uwtable
define void @func3(i32 %age) #0 !dbg !28 {
entry:
  %age.addr = alloca i32, align 4
  store i32 %age, i32* %age.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %age.addr, metadata !31, metadata !DIExpression()), !dbg !32
  %0 = load i32, i32* %age.addr, align 4, !dbg !33
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.2, i32 0, i32 0), i32 %0), !dbg !34
  ret void, !dbg !35
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 !dbg !36 {
entry:
  %p = alloca %struct.Person, align 4
  call void @llvm.dbg.declare(metadata %struct.Person* %p, metadata !39, metadata !DIExpression()), !dbg !48
  %0 = bitcast %struct.Person* %p to i8*, !dbg !48
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %0, i8* align 4 bitcast (%struct.Person* @main.p to i8*), i64 16, i1 false), !dbg !48
  %age = getelementptr inbounds %struct.Person, %struct.Person* %p, i32 0, i32 0, !dbg !49
  call void @func2(i32* %age), !dbg !50
  %name = getelementptr inbounds %struct.Person, %struct.Person* %p, i32 0, i32 1, !dbg !51
  %arraydecay = getelementptr inbounds [10 x i8], [10 x i8]* %name, i32 0, i32 0, !dbg !52
  call void @func1(i8* %arraydecay), !dbg !53
  %age1 = getelementptr inbounds %struct.Person, %struct.Person* %p, i32 0, i32 0, !dbg !54
  %1 = load i32, i32* %age1, align 4, !dbg !54
  call void @func3(i32 %1), !dbg !55
  ret i32 0, !dbg !56
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i1) #4

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { argmemonly nounwind }
attributes #5 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5}
!llvm.ident = !{!6}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "test.c", directory: "/home/yongzhe/llvm-versions/llvm-5.0/lib/Analysis/PDG/test/test_idl_gen")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
!7 = distinct !DISubprogram(name: "func1", scope: !1, file: !1, line: 9, type: !8, isLocal: false, isDefinition: true, scopeLine: 9, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!8 = !DISubroutineType(types: !9)
!9 = !{null, !10}
!10 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!11 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!12 = !DILocalVariable(name: "name", arg: 1, scope: !7, file: !1, line: 9, type: !10)
!13 = !DILocation(line: 9, column: 18, scope: !7)
!14 = !DILocation(line: 10, column: 18, scope: !7)
!15 = !DILocation(line: 10, column: 5, scope: !7)
!16 = !DILocation(line: 11, column: 13, scope: !7)
!17 = !DILocation(line: 11, column: 5, scope: !7)
!18 = !DILocation(line: 12, column: 1, scope: !7)
!19 = distinct !DISubprogram(name: "func2", scope: !1, file: !1, line: 14, type: !20, isLocal: false, isDefinition: true, scopeLine: 14, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!20 = !DISubroutineType(types: !21)
!21 = !{null, !22}
!22 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !23, size: 64)
!23 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!24 = !DILocalVariable(name: "age", arg: 1, scope: !19, file: !1, line: 14, type: !22)
!25 = !DILocation(line: 14, column: 17, scope: !19)
!26 = !DILocation(line: 15, column: 9, scope: !19)
!27 = !DILocation(line: 18, column: 1, scope: !19)
!28 = distinct !DISubprogram(name: "func3", scope: !1, file: !1, line: 20, type: !29, isLocal: false, isDefinition: true, scopeLine: 20, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!29 = !DISubroutineType(types: !30)
!30 = !{null, !23}
!31 = !DILocalVariable(name: "age", arg: 1, scope: !28, file: !1, line: 20, type: !23)
!32 = !DILocation(line: 20, column: 16, scope: !28)
!33 = !DILocation(line: 21, column: 20, scope: !28)
!34 = !DILocation(line: 21, column: 5, scope: !28)
!35 = !DILocation(line: 22, column: 1, scope: !28)
!36 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 24, type: !37, isLocal: false, isDefinition: true, scopeLine: 24, isOptimized: false, unit: !0, variables: !2)
!37 = !DISubroutineType(types: !38)
!38 = !{!23}
!39 = !DILocalVariable(name: "p", scope: !36, file: !1, line: 25, type: !40)
!40 = !DIDerivedType(tag: DW_TAG_typedef, name: "Person", file: !1, line: 7, baseType: !41)
!41 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !1, line: 4, size: 128, elements: !42)
!42 = !{!43, !44}
!43 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !41, file: !1, line: 5, baseType: !23, size: 32)
!44 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !41, file: !1, line: 6, baseType: !45, size: 80, offset: 32)
!45 = !DICompositeType(tag: DW_TAG_array_type, baseType: !11, size: 80, elements: !46)
!46 = !{!47}
!47 = !DISubrange(count: 10)
!48 = !DILocation(line: 25, column: 12, scope: !36)
!49 = !DILocation(line: 26, column: 14, scope: !36)
!50 = !DILocation(line: 26, column: 5, scope: !36)
!51 = !DILocation(line: 27, column: 13, scope: !36)
!52 = !DILocation(line: 27, column: 11, scope: !36)
!53 = !DILocation(line: 27, column: 5, scope: !36)
!54 = !DILocation(line: 28, column: 13, scope: !36)
!55 = !DILocation(line: 28, column: 5, scope: !36)
!56 = !DILocation(line: 29, column: 1, scope: !36)
