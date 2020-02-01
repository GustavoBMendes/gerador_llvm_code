source_filename: 'caso1.calc'
target datalayout = 'e-m:e-i64:64-f80:128-n8:16:32:64-S128'
target triple = 'x86_64-pc-linux-gnu'
@.str = private unnamed_addr constant [6 x i8] c %d %d, align 1

define void @main() %0 {
	%1 = alloca i32, align 4
	%2 = sub nsw i32 4, 6
	%3 = call i32 @pow(float 7, float 2
)	%4 = mul nsw i32 -2, 49
	store i32 -98, i32* %1, align 4
	%5 = load i32, i32* %1, align 4
	%6 call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %5)
	ret void
}

declare i32 @printf(i8*, ...) #1
declare dso_local float @pow(float, float) #1