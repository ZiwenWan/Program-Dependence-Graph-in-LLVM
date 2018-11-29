; ModuleID = 'tracing_arg_info.c'
source_filename = "tracing_arg_info.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.person_t = type { i32, [10 x i8] }

@.str = private unnamed_addr constant [9 x i8] c"new_name\00", align 1
@.str.1 = private unnamed_addr constant [14 x i8] c"New name: %s\0A\00", align 1
@.str.2 = private unnamed_addr constant [17 x i8] c"current name %s\0A\00", align 1
@main.p1 = private unnamed_addr constant %struct.person_t { i32 14, [10 x i8] c"Foo\00\00\00\00\00\00\00" }, align 4

; Function Attrs: noinline nounwind optnone uwtable
define void @f1(i8* %name) #0 !dbg !7 {
entry:
  %name.addr = alloca i8*, align 8
  store i8* %name, i8** %name.addr, align 8
  call void @llvm.dbg.declare(metadata i8** %name.addr, metadata !12, metadata !DIExpression()), !dbg !13
  %0 = load i8*, i8** %name.addr, align 8, !dbg !14
  %call = call i8* @strncpy(i8* %0, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str, i32 0, i32 0), i64 10) #5, !dbg !15
  %1 = load i8*, i8** %name.addr, align 8, !dbg !16
  %call1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.1, i32 0, i32 0), i8* %1), !dbg !17
  ret void, !dbg !18
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind
declare i8* @strncpy(i8*, i8*, i64) #2

declare i32 @printf(i8*, ...) #3

; Function Attrs: noinline nounwind optnone uwtable
define void @f(%struct.person_t* %p) #0 !dbg !19 {
entry:
  %p.addr = alloca %struct.person_t*, align 8
  store %struct.person_t* %p, %struct.person_t** %p.addr, align 8
  call void @llvm.dbg.declare(metadata %struct.person_t** %p.addr, metadata !32, metadata !DIExpression()), !dbg !33
  %0 = load %struct.person_t*, %struct.person_t** %p.addr, align 8, !dbg !34
  %name = getelementptr inbounds %struct.person_t, %struct.person_t* %0, i32 0, i32 1, !dbg !35
  %arraydecay = getelementptr inbounds [10 x i8], [10 x i8]* %name, i32 0, i32 0, !dbg !34
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.2, i32 0, i32 0), i8* %arraydecay), !dbg !36
  %1 = load %struct.person_t*, %struct.person_t** %p.addr, align 8, !dbg !37
  %name1 = getelementptr inbounds %struct.person_t, %struct.person_t* %1, i32 0, i32 1, !dbg !38
  %arraydecay2 = getelementptr inbounds [10 x i8], [10 x i8]* %name1, i32 0, i32 0, !dbg !37
  call void @f1(i8* %arraydecay2), !dbg !39
  ret void, !dbg !40
}

; Function Attrs: noinline nounwind optnone uwtable
define i32 @main() #0 !dbg !41 {
entry:
  %retval = alloca i32, align 4
  %p1 = alloca %struct.person_t, align 4
  store i32 0, i32* %retval, align 4
  call void @llvm.dbg.declare(metadata %struct.person_t* %p1, metadata !44, metadata !DIExpression()), !dbg !45
  %0 = bitcast %struct.person_t* %p1 to i8*, !dbg !45
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %0, i8* align 4 bitcast (%struct.person_t* @main.p1 to i8*), i64 16, i1 false), !dbg !45
  call void @f(%struct.person_t* %p1), !dbg !46
  ret i32 0, !dbg !47
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i1) #4

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { argmemonly nounwind }
attributes #5 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5}
!llvm.ident = !{!6}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "tracing_arg_info.c", directory: "/home/yongzhe/llvm-versions/llvm-5.0/lib/Analysis/PDG/test/test_idl_gen")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{!"clang version 7.0.0 (trunk 323988) (llvm/trunk 323938)"}
!7 = distinct !DISubprogram(name: "f1", scope: !1, file: !1, line: 11, type: !8, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!8 = !DISubroutineType(types: !9)
!9 = !{null, !10}
!10 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!11 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!12 = !DILocalVariable(name: "name", arg: 1, scope: !7, file: !1, line: 11, type: !10)
!13 = !DILocation(line: 11, column: 15, scope: !7)
!14 = !DILocation(line: 12, column: 13, scope: !7)
!15 = !DILocation(line: 12, column: 5, scope: !7)
!16 = !DILocation(line: 13, column: 30, scope: !7)
!17 = !DILocation(line: 13, column: 5, scope: !7)
!18 = !DILocation(line: 14, column: 1, scope: !7)
!19 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 16, type: !20, isLocal: false, isDefinition: true, scopeLine: 16, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!20 = !DISubroutineType(types: !21)
!21 = !{null, !22}
!22 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !23, size: 64)
!23 = !DIDerivedType(tag: DW_TAG_typedef, name: "Person", file: !1, line: 9, baseType: !24)
!24 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "person_t", file: !1, line: 4, size: 128, elements: !25)
!25 = !{!26, !28}
!26 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !24, file: !1, line: 5, baseType: !27, size: 32)
!27 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!28 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !24, file: !1, line: 6, baseType: !29, size: 80, offset: 32)
!29 = !DICompositeType(tag: DW_TAG_array_type, baseType: !11, size: 80, elements: !30)
!30 = !{!31}
!31 = !DISubrange(count: 10)
!32 = !DILocalVariable(name: "p", arg: 1, scope: !19, file: !1, line: 16, type: !22)
!33 = !DILocation(line: 16, column: 16, scope: !19)
!34 = !DILocation(line: 17, column: 33, scope: !19)
!35 = !DILocation(line: 17, column: 36, scope: !19)
!36 = !DILocation(line: 17, column: 5, scope: !19)
!37 = !DILocation(line: 18, column: 8, scope: !19)
!38 = !DILocation(line: 18, column: 11, scope: !19)
!39 = !DILocation(line: 18, column: 5, scope: !19)
!40 = !DILocation(line: 19, column: 1, scope: !19)
!41 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 21, type: !42, isLocal: false, isDefinition: true, scopeLine: 21, isOptimized: false, unit: !0, variables: !2)
!42 = !DISubroutineType(types: !43)
!43 = !{!27}
!44 = !DILocalVariable(name: "p1", scope: !41, file: !1, line: 22, type: !23)
!45 = !DILocation(line: 22, column: 12, scope: !41)
!46 = !DILocation(line: 23, column: 5, scope: !41)
!47 = !DILocation(line: 24, column: 5, scope: !41)
