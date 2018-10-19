; ModuleID = 'tracing_arg_info.c'
source_filename = "tracing_arg_info.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.person_t = type { i32, [10 x i8] }

@.str = private unnamed_addr constant [9 x i8] c"new_name\00", align 1
@.str.1 = private unnamed_addr constant [14 x i8] c"New name: %s\0A\00", align 1
@.str.2 = private unnamed_addr constant [17 x i8] c"current name %s\0A\00", align 1
@main.p1 = private unnamed_addr constant %struct.person_t { i32 14, [10 x i8] c"Foo\00\00\00\00\00\00\00" }, align 4

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @f1(i8*) #0 !dbg !8 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  call void @llvm.dbg.declare(metadata i8** %2, metadata !13, metadata !14), !dbg !15
  %3 = load i8*, i8** %2, align 8, !dbg !16
  %4 = load i8*, i8** %2, align 8, !dbg !16
  %5 = call i64 @llvm.objectsize.i64.p0i8(i8* %4, i1 false, i1 true), !dbg !16
  %6 = call i8* @__strncpy_chk(i8* %3, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str, i32 0, i32 0), i64 10, i64 %5) #5, !dbg !16
  %7 = load i8*, i8** %2, align 8, !dbg !17
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.1, i32 0, i32 0), i8* %7), !dbg !18
  ret void, !dbg !19
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind
declare i8* @__strncpy_chk(i8*, i8*, i64, i64) #2

; Function Attrs: nounwind readnone speculatable
declare i64 @llvm.objectsize.i64.p0i8(i8*, i1, i1) #1

declare i32 @printf(i8*, ...) #3

; Function Attrs: noinline nounwind optnone ssp uwtable
define void @f(%struct.person_t*) #0 !dbg !20 {
  %2 = alloca %struct.person_t*, align 8
  store %struct.person_t* %0, %struct.person_t** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.person_t** %2, metadata !33, metadata !14), !dbg !34
  %3 = load %struct.person_t*, %struct.person_t** %2, align 8, !dbg !35
  %4 = getelementptr inbounds %struct.person_t, %struct.person_t* %3, i32 0, i32 1, !dbg !36
  %5 = getelementptr inbounds [10 x i8], [10 x i8]* %4, i32 0, i32 0, !dbg !35
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.2, i32 0, i32 0), i8* %5), !dbg !37
  %7 = load %struct.person_t*, %struct.person_t** %2, align 8, !dbg !38
  %8 = getelementptr inbounds %struct.person_t, %struct.person_t* %7, i32 0, i32 1, !dbg !39
  %9 = getelementptr inbounds [10 x i8], [10 x i8]* %8, i32 0, i32 0, !dbg !38
  call void @f1(i8* %9), !dbg !40
  ret void, !dbg !41
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 !dbg !42 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.person_t, align 4
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata %struct.person_t* %2, metadata !45, metadata !14), !dbg !46
  %3 = bitcast %struct.person_t* %2 to i8*, !dbg !46
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* bitcast (%struct.person_t* @main.p1 to i8*), i64 16, i32 4, i1 false), !dbg !46
  call void @f(%struct.person_t* %2), !dbg !47
  ret i32 0, !dbg !48
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #4

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { argmemonly nounwind }
attributes #5 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 5.0.2 (tags/RELEASE_502/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "tracing_arg_info.c", directory: "/Users/yongzhehuang/Documents/pdg-projects/pdg-llvm5.0/test/test_idl_gen")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"wchar_size", i32 4}
!6 = !{i32 7, !"PIC Level", i32 2}
!7 = !{!"clang version 5.0.2 (tags/RELEASE_502/final)"}
!8 = distinct !DISubprogram(name: "f1", scope: !1, file: !1, line: 11, type: !9, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{null, !11}
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!12 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!13 = !DILocalVariable(name: "name", arg: 1, scope: !8, file: !1, line: 11, type: !11)
!14 = !DIExpression()
!15 = !DILocation(line: 11, column: 15, scope: !8)
!16 = !DILocation(line: 12, column: 5, scope: !8)
!17 = !DILocation(line: 13, column: 30, scope: !8)
!18 = !DILocation(line: 13, column: 5, scope: !8)
!19 = !DILocation(line: 14, column: 1, scope: !8)
!20 = distinct !DISubprogram(name: "f", scope: !1, file: !1, line: 16, type: !21, isLocal: false, isDefinition: true, scopeLine: 16, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!21 = !DISubroutineType(types: !22)
!22 = !{null, !23}
!23 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !24, size: 64)
!24 = !DIDerivedType(tag: DW_TAG_typedef, name: "Person", file: !1, line: 9, baseType: !25)
!25 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "person_t", file: !1, line: 4, size: 128, elements: !26)
!26 = !{!27, !29}
!27 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !25, file: !1, line: 5, baseType: !28, size: 32)
!28 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!29 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !25, file: !1, line: 6, baseType: !30, size: 80, offset: 32)
!30 = !DICompositeType(tag: DW_TAG_array_type, baseType: !12, size: 80, elements: !31)
!31 = !{!32}
!32 = !DISubrange(count: 10)
!33 = !DILocalVariable(name: "p", arg: 1, scope: !20, file: !1, line: 16, type: !23)
!34 = !DILocation(line: 16, column: 16, scope: !20)
!35 = !DILocation(line: 17, column: 33, scope: !20)
!36 = !DILocation(line: 17, column: 36, scope: !20)
!37 = !DILocation(line: 17, column: 5, scope: !20)
!38 = !DILocation(line: 18, column: 8, scope: !20)
!39 = !DILocation(line: 18, column: 11, scope: !20)
!40 = !DILocation(line: 18, column: 5, scope: !20)
!41 = !DILocation(line: 19, column: 1, scope: !20)
!42 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 21, type: !43, isLocal: false, isDefinition: true, scopeLine: 21, isOptimized: false, unit: !0, variables: !2)
!43 = !DISubroutineType(types: !44)
!44 = !{!28}
!45 = !DILocalVariable(name: "p1", scope: !42, file: !1, line: 22, type: !24)
!46 = !DILocation(line: 22, column: 12, scope: !42)
!47 = !DILocation(line: 23, column: 5, scope: !42)
!48 = !DILocation(line: 24, column: 5, scope: !42)
