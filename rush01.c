#include <stdlib.h>
#include <iostream>

//статус клетки - свободна/заполнена
enum e_cell_status
{
    CELL_VALUE_UNKNOWN = 0,
    CELL_VALUE_FIXED = 1
};

//статус конкретного числа для клетки - запрещено/разрешено
enum e_int_status
{
    NUMBER_FORBIDDEN = 0,
    NUMBER_POSSIBLE = 1
};

//результат подготовки массива данных
enum e_prepare_result
{
    PREPARE_OK = 0,
    PREPARE_FAIL = 1
};

//структура для обработки отдельной клетки
struct s_cell
{
    e_cell_status cell_status;//статус значения в клетке
    int fixed_value;//значение в клетке - если есть (иначе -1)
    int possible_value_quantity;//количество возможных значений
    e_int_status possible_values[9];//значения 0-8 (~1-9) с "признаком возможности"
};

//статус - существуют ли зафиксированные клетки
enum e_fixed_exist
{
    FIXED_EXIST,
    FIXED_NOT_EXIST
};

//функция размечает все клетки, находящиеся на одной горизонтали с клеткой (i,j),
//что в них не может быть число current_number
e_prepare_result mark_horizontal(s_cell*** ptr_result, int i, int j, int current_number)
{
    int k;

    k = 0;
    while (k < 9)
    {
        if (k != j)
        {
            if ((ptr_result[0][i][k].cell_status == CELL_VALUE_FIXED)
                    && (ptr_result[0][i][k].fixed_value == current_number))
                return PREPARE_FAIL;
            else
            {
                if ((ptr_result[0][i][k].cell_status != CELL_VALUE_FIXED)
                        &&(ptr_result[0][i][k].possible_values[current_number] == NUMBER_POSSIBLE))
                {
                    ptr_result[0][i][k].possible_values[current_number] = NUMBER_FORBIDDEN;
                    ptr_result[0][i][k].possible_value_quantity -= 1;
                    if (ptr_result[0][i][k].possible_value_quantity < 1)
                        return PREPARE_FAIL;
                }
            }
        }
        k += 1;
    }
    return PREPARE_OK;
}

//функция размечает все клетки, находящиеся на одной вертикали с клеткой (i,j),
//что в них не может быть число current_number
e_prepare_result mark_vertical(s_cell*** ptr_result, int i, int j, int current_number)
{
    int m;

    m = 0;
    while (m < 9)
    {
        if (m != i)
        {
            if ((ptr_result[0][m][j].cell_status == CELL_VALUE_FIXED)
                    && (ptr_result[0][m][j].fixed_value == current_number))
            {
                return PREPARE_FAIL;
            }
            else
            {
                if ((ptr_result[0][m][j].cell_status != CELL_VALUE_FIXED)
                        &&(ptr_result[0][m][j].possible_values[current_number] == NUMBER_POSSIBLE))
                {
                    ptr_result[0][m][j].possible_values[current_number] = NUMBER_FORBIDDEN;
                    ptr_result[0][m][j].possible_value_quantity -= 1;
                    if (ptr_result[0][m][j].possible_value_quantity < 1)
                        return PREPARE_FAIL;
                }

            }
        }
        m += 1;
    }
    return PREPARE_OK;
}

//функция размечает все клетки, находящиеся в одном районе 3х3 с клеткой (i,j),
//что в них не может быть число current_number
e_prepare_result mark_3x3(s_cell*** ptr_result, int i, int j, int current_number)
{
    int k;
    int m;

    k = (j / 3) * 3;
    while (k < ((j / 3) * 3 + 3))
    {
        m = (i / 3) * 3;
        while (m < ((i / 3) * 3 + 3))
        {
            if ((k != j)||(m != i))
                if ((ptr_result[0][m][k].cell_status != CELL_VALUE_FIXED)
                        &&(ptr_result[0][m][k].possible_values[current_number] == NUMBER_POSSIBLE))
                {
                    ptr_result[0][m][k].possible_values[current_number] = NUMBER_FORBIDDEN;
                    ptr_result[0][m][k].possible_value_quantity -= 1;
                    if (ptr_result[0][m][k].possible_value_quantity < 1)
                        return PREPARE_FAIL;
                }
            m += 1;
        }
        k += 1;
    }
    return PREPARE_OK;
}

//общая функция для разметки данной клетки (помещения в нее current_number)
//и разметки всех соседних по горизонтали-вертикали-району клеток
e_prepare_result mark_cell(s_cell*** ptr_result, int i, int j, int current_number)
{
    if (ptr_result[0][i][j].possible_values[current_number] == NUMBER_FORBIDDEN)
        return PREPARE_FAIL;

    ptr_result[0][i][j].cell_status = CELL_VALUE_FIXED;
    ptr_result[0][i][j].fixed_value = current_number;
    ptr_result[0][i][j].possible_value_quantity = 0;

    e_prepare_result mark_horizontal_result = mark_horizontal(ptr_result, i, j, current_number);
    if (mark_horizontal_result == PREPARE_FAIL)
        return PREPARE_FAIL;
    e_prepare_result mark_vertical_result = mark_vertical(ptr_result, i, j, current_number);
    if (mark_vertical_result == PREPARE_FAIL)
        return PREPARE_FAIL;
    e_prepare_result mark_3x3_result = mark_3x3(ptr_result, i, j, current_number);
    if (mark_3x3_result == PREPARE_FAIL)
        return PREPARE_FAIL;
    return PREPARE_OK;
}

//функция создает первоначальный массив 9х9 для хранения информации обо всех клетках
void create_array(s_cell*** ptr_result_array)
{
    int i;
    int j;
    s_cell **result_array;

    result_array = (s_cell**)malloc(sizeof(s_cell*) * 9);
    i = 0;
    while(i < 9)
    {
        result_array[i] = (s_cell*)malloc(sizeof(s_cell) * 9);
        i += 1;
    }
    *ptr_result_array = result_array;
}

//функция инициализирует одну клетку: значение неизвестно, все числа допустимы
s_cell initialize_one_cell()
{
    s_cell ij_cell;
    int i;

    ij_cell.cell_status = CELL_VALUE_UNKNOWN;
    ij_cell.fixed_value = -1;
    ij_cell.possible_value_quantity = 9;
    while (i < 9)
    {
        ij_cell.possible_values[i] = NUMBER_POSSIBLE;
        i += 1;
    }
    return (ij_cell);
}

//функция инициализирует первоначальный массив 9х9 для хранения информации обо всех клетках
void create_and_initialize_array(s_cell*** ptr_result_array)
{
    int i;
    int j;
    s_cell **result_array;

    create_array(&result_array);
    i = 0;
    while (i < 9)
    {
        j = 0;
        while (j < 9)
        {
            result_array[i][j] = initialize_one_cell();
            j += 1;
        }
        i += 1;
    }
    *ptr_result_array = result_array;
}

//функция заполняет массив 9х9 в соответствии с входной информацией
e_prepare_result prepare_array(char **ptr_strings, s_cell*** ptr_result_array)
{
    int i;
    int j;
    e_prepare_result ij_result;
    s_cell **result_array;

    create_and_initialize_array(&result_array);
    i = 0;
    while (i < 9)
    {
        j = 0;
        while (j < 9)
        {
            if(ptr_strings[i][j] != '.')
            {
                ij_result = mark_cell(&result_array, i, j, ptr_strings[i][j] - '0' - 1);
                if (ij_result == PREPARE_FAIL)
                    return PREPARE_FAIL;
            }
            j += 1;
        }
        i += 1;
    }
    *ptr_result_array = result_array;
    return PREPARE_OK;
}

//функция создает объект - копию массива для отправки в "дочернюю" рекурсивную функцию
s_cell** create_copy_array(s_cell*** array)
{
    s_cell **result;
    int i;
    int j;

    result = (s_cell**)malloc(sizeof(s_cell*) * 9);
    i = 0;
    while (i < 9)
    {
        result[i] = (s_cell*)malloc(sizeof(s_cell) * 9);
        j = 0;
        while (j < 9)
        {
            result[i][j] = (*array)[i][j];
            j += 1;
        }
        i += 1;
    }
    return result;
}

//функция освобождает память, выделенную под массив
void free_array(s_cell*** array)
{
    int i;

    i = 0;
    while (i < 9)
    {
       free((*array)[i]);
       i += 1;
    }
    free(*array);
}

//функция проверяет, что хотя бы одна клетка является FIXED (для обработки случая, когда на вход все "...")
e_fixed_exist check_fixed_exist(s_cell** result)
{
    int i;
    int j;
    e_fixed_exist fixed_cells = FIXED_NOT_EXIST;

    i = 0;
    while (i < 9)
    {
        j = 0;
        while (j < 9)
        {
            if (result[i][j].cell_status == CELL_VALUE_FIXED)
                fixed_cells = FIXED_EXIST;
            j += 1;
        }
        i += 1;
    }
    return fixed_cells;
}

//функция находит клетку, в которой минимальное отличное от 0 число вариантов заполнения
void find_min_quantity_unknown(s_cell** result, int *ptr_min_quantity, int *ptr_i, int *ptr_j)
{
    int min_quantity;
    int i;
    int j;

    min_quantity = 9;
    i = 0;
    while (i < 9)
    {
        j = 0;
        while (j < 9)
        {
            if ((result[i][j].cell_status == CELL_VALUE_UNKNOWN)
                    &&(min_quantity > result[i][j].possible_value_quantity))
            {
                *ptr_i = i;
                *ptr_j = j;
                min_quantity = result[i][j].possible_value_quantity;
            }
            j += 1;
        }
        i += 1;
    }
    *ptr_min_quantity = min_quantity;
}

int analyze_array(s_cell** current_array, s_cell*** ptr_result);

//функция выполняет анализ разрешенных комбинаций
//для заданной точки
//возвращается количество комбинаций, "доведенных до конца", т.е. приведших к полному заполнению таблицы
//в номинальном случае 1, иначе 0 или больше 1
int analyze_array_point(s_cell** current_array, int i_with_min, int j_with_min, s_cell*** ptr_result)
{
    int i;
    int j;
    s_cell** copy;
    int good_combi_quantity;
    int copy_result;

    good_combi_quantity = 0;
    i = 0;
    while (i < 9)
    {
        if( current_array[i_with_min][j_with_min].possible_values[i] == NUMBER_POSSIBLE )
        {
            copy = create_copy_array(&current_array);
            if (mark_cell(&copy, i_with_min, j_with_min, i) == PREPARE_OK)
            {
                copy_result = analyze_array(copy, ptr_result);
                good_combi_quantity += copy_result;
            }
            free_array(&copy);
        }
        i += 1;
    }
    return (good_combi_quantity);
}


//функция выполняет анализ массива
//если в массиве не осталось точек, для которых возможны варианты заполнения (все клетки FIXED), то GOOD_COMBI
//иначе рекурсивно вызываются аналогичные функции для различных комбинаций точки,
//которая выбрана по минимальному количеству вариантов значений
//возвращается количество комбинаций, "доведенных до конца", т.е. приведших к полному заполнению таблицы
//в номинальном случае 1, иначе 0 или больше 1
int analyze_array(s_cell** current_array, s_cell*** ptr_result)
{
    int min_quantity;
    int i_with_min;
    int j_with_min;
    int good_combi_quantity;

    find_min_quantity_unknown(current_array, &min_quantity, &i_with_min, &j_with_min);
    if (min_quantity == 9)
    {
        *ptr_result = create_copy_array(&current_array);
        good_combi_quantity = 1;
    }
    else
    {
        good_combi_quantity = analyze_array_point(current_array, i_with_min, j_with_min, ptr_result);
    }
    return (good_combi_quantity);
}

//функция печатает массив с результатом
void print_result_array(s_cell** final_result)
{
    int i;
    int j;

    i = 0;
    while (i < 9)
    {
        j = 0;
        while (j < 9)
        {
            printf("%d ", final_result[i][j].fixed_value + 1);
            j += 1;
        }
        printf("\n");
        i += 1;
    }
}

//функция печатает сообщение об ошибке
void print_error()
{
    printf("Error\n");
}

//основная расчетная функция
void calc_sudoku(char **ss)
{
    s_cell** result;
    e_prepare_result combine_result = prepare_array(ss, &result);
    if(check_fixed_exist(result) != FIXED_EXIST)
        print_error();
    else
    {
        int analyze_result = 0;
        if (combine_result == PREPARE_OK)
        {
            s_cell** final_result;
            analyze_result = analyze_array(result, &final_result);
            if (analyze_result == 1)
                print_result_array(final_result);
            else
                print_error();
        }
        else
            print_error();
    }
    free_array(&result);
}

//main с примером из задания
int main()
{
    char *s1 = "9...7....";
    char *s2 = "2...9..53";
    char *s3 = ".6..124..";
    char *s4 = "84...1.9.";
    char *s5 = "5.....8..";
    char *s6 = ".31..4...";
    char *s7 = "..37..68.";
    char *s8 = ".9..5.741";
    char *s9 = "47.......";
    char **ss = (char**)malloc(sizeof(char*) * 9);
    ss[0] = s1; ss[1] = s2; ss[2] = s3; ss[3] = s4; ss[4] = s5; ss[5] = s6; ss[6] = s7; ss[7] = s8; ss[8] = s9;

    calc_sudoku(ss);

    return 0;
}
