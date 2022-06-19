#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <shellapi.h>

#define MAX_BUTTONS 30
#define zero_ASCII 48
#define DEF_BUFFER_SIZE 100
#define calc_display_id 101

HWND main_window_handle = NULL;
HWND main_calc_display_label = NULL;
LPSTR main_window_class_name = "Main Window Class";
LPSTR main_window_name = "Windows Calculator";
HDC dc_handle = NULL;
int def_w_left = 340;
int def_w_top = 100;
int def_w_height = 540;
int default_w_width = 640;
char* buttons_text_list[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "+", "-", "x", "÷", ".", "%", "=", "Back", "Clear"};

typedef struct ButtonGrid
{
    int grid_id;

    int buttons_count;

    int h_spacing;

    int v_spacing;

    int grid_left;

    int grid_top;

    int buttons_per_row;

    HWND buttons_list[MAX_BUTTONS];

    int button_width;

    int button_height;

}ButtonGrid;

ButtonGrid calculator_buttons = {0};

char convert_int_to_char(int number)
{
    char new_char = number + '0';

    return new_char;
}

ButtonGrid create_button_grid(int grid_id, int buttons_count, int h_spacing, int v_spacing, int grid_left, int grid_top, int buttons_per_row, int button_width, int button_height, HWND parent_window_handle)
{
    ButtonGrid new_button_grid = {grid_id, buttons_count, h_spacing, v_spacing, grid_left, grid_top, buttons_per_row, {0}, button_width, button_height};

    DWORD button_styles = BS_NOTIFY | BS_TEXT | BS_DEFPUSHBUTTON | WS_BORDER | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE;

    for (int i = 0; i < buttons_count; i++)
    {
        int buttons_created = 0;

        int btns_per_row = 0;

        int current_row = 1;

        int u_id = 150;

        while (buttons_created < new_button_grid.buttons_count)
        {
            while (btns_per_row < new_button_grid.buttons_per_row && buttons_created < new_button_grid.buttons_count)
            {
                new_button_grid.buttons_list[buttons_created] = CreateWindowA("Button", buttons_text_list[buttons_created], button_styles, grid_left + btns_per_row * (h_spacing + button_width), grid_top + current_row * (v_spacing + button_height), button_width, button_height, parent_window_handle , (HMENU)u_id, GetModuleHandle(NULL), NULL);

                btns_per_row ++;

                buttons_created ++;

                u_id++;
            }

            btns_per_row = 0;

            current_row ++;
        }
    }

    return new_button_grid;
}

void label_calculator_buttons(ButtonGrid* btn_grid_ptr)
{
    /*char current_symbol[] = "A";

    for (int i = 0; i < btn_grid_ptr->buttons_count; i++)
    {
        if (i < 9)
        {
            current_symbol[0] = i + 1 + '0';
        }else if (i == 9)
        {
            current_symbol[0] = '0';
        }else if (i == 10)
        {
            current_symbol[0] = '+';
        }else if (i == 11)
        {
            current_symbol[0] = '=';
        }
        SetWindowTextA(btn_grid_ptr->buttons_list[i], current_symbol);

        SendMessage(btn_grid_ptr->buttons_list[i], WM_SETTEXT, 0, (LPARAM)(current_symbol));
    }*/
}

void InitMainWindowControls(HINSTANCE h_instance)
{
    DWORD calc_display_style = SS_SUNKEN | SS_CENTER | WS_CHILD | WS_VISIBLE;

    main_calc_display_label = CreateWindowA("Static", "Do some calculations here.", calc_display_style, 100, 40, 440, 60, main_window_handle, (HMENU)calc_display_id, h_instance, NULL);

    calculator_buttons = create_button_grid(1, 19, 100, 20, 130, 80, 3, 60, 30, main_window_handle);

    label_calculator_buttons(&calculator_buttons);
}

LRESULT WndProc(HWND h_window, UINT u_msg, WPARAM w_param, LPARAM l_param)
{
    switch (u_msg)
    {
        case WM_COMMAND:
        {
            if (main_calc_display_label != NULL)
            {
                char calc_display_text[DEF_BUFFER_SIZE] = "Placeholder text.";

                SendMessage(main_calc_display_label, WM_GETTEXT, DEF_BUFFER_SIZE, (LPARAM)&calc_display_text);

                /*The issue occurs only when a char* (which LPSTR is typedefined as) is used, however using a char array with
                a fixed length (e.g. 100) solves this issue (Text not getting copied into the buffer initially but then also
                a Memory Access Violation error being thrown upon calling the SendMessage function above).*/

                if(strcmp(calc_display_text, "Do some calculations here.") == 0)
                {
                    *calc_display_text = '\0';

                    SetWindowTextA(main_calc_display_label, calc_display_text);
                }

                for (int i = 0; i < calculator_buttons.buttons_count; i++)
                {
                    int button_state = SendMessage(calculator_buttons.buttons_list[i], BM_GETSTATE, 0, 0);

                    printf("Button state: %d\n", button_state);

                    if (LOWORD(w_param) == GetDlgCtrlID(calculator_buttons.buttons_list[i]))
                    {
                        strcat(calc_display_text, buttons_text_list[i]);

                        SetWindowTextA(main_calc_display_label, calc_display_text);
                    }
                }
            }

            break;
        }
        case WM_CLOSE:
        {
            DestroyWindow(h_window);

            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);

            break;
        }
        default:
        {
            return DefWindowProc(h_window, u_msg, w_param, l_param);
        }
    }

    return 0;
}

void InitMainWindow(HINSTANCE h_instance)
{
    WNDCLASSA main_window_class = {0};

    main_window_class.style = CS_DBLCLKS;

    main_window_class.lpfnWndProc = WndProc;

    main_window_class.hInstance = h_instance;

    main_window_class.hbrBackground = (HBRUSH)COLOR_WINDOW;

    main_window_class.hIcon = LoadIconA(NULL, IDI_WINLOGO);

    main_window_class.hCursor = LoadCursorA(NULL, IDC_ARROW);

    main_window_class.lpszClassName = main_window_class_name;

    RegisterClass(&main_window_class);

    DWORD main_window_style = WS_CAPTION | WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX ;

    main_window_handle = CreateWindow(main_window_class_name, main_window_name, main_window_style, def_w_left, def_w_top, default_w_width, def_w_height, NULL, NULL, h_instance, NULL);

    dc_handle = GetDC(main_window_handle);

    RECT main_window_rect;

    GetWindowRect(main_window_handle, &main_window_rect);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    InitMainWindow(hInstance);

    InitMainWindowControls(hInstance);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        TranslateMessage(&msg);

        DispatchMessage(&msg);
    }

    return 0;
}
