source_filename: 'caso3.calc'
target datalayout = 'e-m:e-i64:64-f80:128-n8:16:32:64-S128'
target triple = 'x86_64-pc-linux-gnu'
@.str = private unnamed_addr constant [6 x i8] c %d %d, align 1

define void @main() %0 {
	%1 = add nsw i32 5, 7
	%2 = mul nsw i32 5, 12
	%3 call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %2)
	ret void
}

declare i32 @printf(i8*, ...) #1
