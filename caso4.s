source_filename: 'caso4.calc'
target datalayout = 'e-m:e-i64:64-f80:128-n8:16:32:64-S128'
target triple = 'x86_64-pc-linux-gnu'
@.str = private unnamed_addr constant [6 x i8] c %d %d, align 1

define void @main() %0 {
	%1 = alloca i32, align 4
	%2 = add nsw i32 20, 15
	store i32 35, i32* %1, align 4
	%3 = alloca i32, align 4
	store i32 5, i32* %3, align 4
	%4 = load i32, i32* %1, align 4
	%5 call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %4)
	%6 = alloca i32, align 4
	%7 = add nsw i32 30, 15
	%8 = load i32, i32* %3, align 4
	%9 = add nsw i32 45, 5
	%10 = load i32, i32* %1, align 4
	%11 = add nsw i32 50, 35
	store i32 85, i32* %6, align 4
	%12 = load i32, i32* %1, align 4
	%13 = load i32, i32* %6, align 4
	%14 = add nsw i32 35, 85
	%15 call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %14)
	ret void
}

declare i32 @printf(i8*, ...) #1
