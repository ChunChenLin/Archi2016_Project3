addi $4 $0 0xffff
addi $8 $0 1
add $7 $7 $0
addi $6 $0 0xa000
beq $7 $6 14
add $8 $8 $8
lw $2 0($1)
sw $2 12($1)
add $5 $4 $8
addi $7 $7 1
lw $2 0($1)
sw $2 12($1)
add $5 $4 $8
add $1 $0 $0
addi $2 $2 0xffff
sw $2 12($1)
lw $3 0($1)
addi $1 $1 4
j 4
add $1 $0 $0
add $2 $0 $0
add $3 $0 $0
add $4 $0 $0
add $5 $0 $0
add $6 $0 $0
add $7 $0 $0
add $8 $0 $0
lw $1 0($0)
addi $4 $0 4
addi $2 $0 1
add $3 $4 $4
add $7 $4 $0
beq $1 $0 9
add $8 $7 $4
lw $9 0($7)
lw $10 0($8)
sw $10 0($7)
sw $9  0($8)
add $8 $8 $3
add $7 $7 $3
sub $1 $1 $2
j 32
lw $1 0($0)
add $5 $0 $4
add $1 $1 $1
beq $1 $0 4
lw $11 0($5)
add $5 $5 $4
sub $1 $1 $2
j 45 
halt
halt
halt
halt
halt
