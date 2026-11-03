# Kiến trúc ứng dụng đèn giao thông

Sơ đồ FSM tổng thể có thể chỉnh sửa: [overall-fsm.drawio](overall-fsm.drawio).
Sơ đồ thể hiện ba chế độ và trạng thái nội bộ; ở chế độ MANUAL, đề xuất hiện
tại là chỉ chuyển pha bằng BTN2, không tự đếm ngược.

Thiết kế đã thống nhất: ba FSM độc lập được gọi ngang hàng trong vòng lặp
`main`; mỗi thời điểm chỉ FSM ứng với chế độ hiện tại được điều khiển đèn.
Kiến trúc này đã được triển khai trong src/fsm*.c và src/global.h/c.

## Phân chia module

| Module | Trách nhiệm |
| --- | --- |
| `global.h/c` | Chế độ hiện tại, cấu hình thời lượng, object đèn dùng chung và ID timer không trùng nhau. Không chứa logic vận hành. Con trỏ `app` được định nghĩa một lần trong `.c`, khai báo `extern` trong `.h`; trỏ đến context `fsm_t` hiện có của `main.c`. |
| `software_timer.h/c` | Nhận tick TIM2 và cung cấp cờ hết hạn. Không biết chế độ hay quy tắc đèn. |
| `button.h/c` | Chống dội và phát sự kiện nhấn/giữ. Không tự đổi chế độ. |
| `traffic_light.h/c` | Quản lý từng object đèn: khởi tạo, đặt màu, đặt thời gian, tăng/giảm thời gian và cập nhật GPIO tương ứng. |
| `fsm_automatic.h/c` | Quản lý pha hai hướng, đếm giây và tự chuyển pha khi hết thời gian. |
| `fsm_manual.h/c` | Quản lý pha hai hướng, chuyển pha theo sự kiện nút. |
| `fsm_setting.h/c` | Quản lý mục đang chỉnh, giá trị tạm, xác nhận cấu hình và nhấp nháy đèn. |
| `seg7_display.h/c` | Quản lý bộ đệm và quét hiển thị, phục vụ mọi chế độ. |
| `main.c` | Khởi tạo, thu thập sự kiện nút, điều phối đổi chế độ, gọi ba FSM cùng level và phục vụ hiển thị. |

## Giới hạn của traffic_light

API thao tác trên một object, ví dụ:

```c
void TRAFFIC_LIGHT_Init(traffic_light_t *light);
void TRAFFIC_LIGHT_SetColor(traffic_light_t *light, light_state_t color);
void TRAFFIC_LIGHT_SetTime(traffic_light_t *light, uint16_t seconds);
void TRAFFIC_LIGHT_IncreaseTime(traffic_light_t *light);
void TRAFFIC_LIGHT_DecreaseTime(traffic_light_t *light);
```

Đây là giao diện minh họa; tham số khởi tạo cụ thể theo cấu trúc GPIO của dự án.
`SetColor` cập nhật màu và GPIO của một cụm đèn. Tăng thời gian phải chặn tràn;
giảm thời gian dừng ở 0. Về 0 không tự chuyển màu.

Module không cung cấp API chạy chu kỳ, chuyển pha hay phối hợp hai hướng;
không đọc nút và không sở hữu timer. Toàn bộ quy tắc vận hành thuộc FSM,
kể cả bảo đảm hai hướng không cùng xanh và chuyển pha đúng thứ tự.

## Ba FSM ngang hàng

Luồng xử lý khái niệm trong `main`:

```text
while (1):
    phục vụ quét nút theo chu kỳ
    đọc và xóa cờ nút một lần thành snapshot sự kiện
    xử lý yêu cầu đổi chế độ và khởi tạo FSM đích nếu cần

    FSM_Automatic_Run()
    FSM_Manual_Run()
    FSM_Setting_Run()

    phục vụ quét hiển thị theo chu kỳ
```

Mỗi hàm `Run` kiểm tra chế độ trước khi xử lý:

```c
void FSM_Automatic_Run(void)
{
    if (mode != MODE_AUTOMATIC)
        return;

    /* Xử lý pha, timer và các object đèn. */
}
```

Ba lời gọi thực thi tuần tự, không cần RTOS. Không FSM nào gọi FSM khác.
Trạng thái nội bộ được giữ riêng trong từng file `.c`: pha của AUTO, pha của
MANUAL, mục cài đặt/giá trị tạm/trạng thái nhấp nháy của SETTING.

## Chuyển chế độ và sự kiện

- Chỉ phần điều phối trước ba lời gọi FSM được thay đổi `mode`.
- Khi vào chế độ, gọi `FSM_Automatic_Init`, `FSM_Manual_Init` hoặc
  `FSM_Setting_Init` để đặt trạng thái, timer và đầu ra ban đầu; giữ cấu hình
  thời lượng đã lưu.
- Mỗi FSM dùng timer riêng; khi vào chế độ phải đặt lại timer và cờ cũ.
- Các FSM chỉ đọc snapshot sự kiện của lượt hiện tại, không tự đọc/xóa cờ nút.
- Nếu lượt đó đổi chế độ, không chuyển tiếp sự kiện thao tác cho FSM đích,
  tránh một lần nhấn vừa đổi chế độ vừa thực hiện thao tác trong chế độ mới.
- Quét hiển thị chạy độc lập với chế độ. Ngắt TIM2 chỉ cập nhật software timer;
  logic vận hành chạy trong vòng lặp chính.

## Hành vi cơ sở đề xuất

- BTN1: `AUTO → MANUAL → SETTING → AUTO`.
- AUTO: tự chạy chuỗi pha và hiển thị thời gian còn lại.
- MANUAL: BTN2 chuyển một pha cho mỗi lần nhấn, không lặp khi giữ; BTN3 chưa có chức năng. Hiển thị thời lượng ban đầu của pha và giữ nguyên đến lần chuyển pha tiếp theo.
- SETTING: BTN2 tăng giá trị; BTN3 lưu rồi chuyển mục `đỏ → vàng → xanh`;
  BTN1 thoát và bỏ giá trị chưa lưu.

Chuỗi pha của hai hướng A/B:

```text
A xanh / B đỏ → A vàng / B đỏ → A đỏ / B xanh → A đỏ / B vàng → lặp lại
```

Với hai hướng có cùng thời lượng, cấu hình tuân theo `đỏ = xanh + vàng`.
Sửa đỏ điều chỉnh xanh và giữ vàng. FSM SETTING chỉ lưu cấu hình hợp lệ,
trong đó mỗi thời lượng nằm trong 1–99 giây.

## Triển khai hiện tại

Đã tách ba FSM vào `src/fsm_automatic.c`, `src/fsm_manual.c`, `src/fsm_setting.c`.
`src/fsm.c` cung cấp khởi tạo, snapshot sự kiện, điều phối chế độ, cập nhật bộ đệm
và thao tác xuất pha dùng chung. Khai báo API nằm trong `src/fsm.h`.
`main` gọi ba FSM ngang hàng; chỉ sửa USER CODE trong thân hàm `main`.
CMake và self-check đã được cập nhật. Cấu hình lưu trong RAM, reset về xanh 2 s, vàng 3 s.
AUTO giữ cách hiển thị cũ N-1..0; SETTING hiển thị 02/03/04 và giá trị đang sửa.
Timer 0: AUTO, 1: quét LED 7 đoạn, 2: đèn báo, 3: quét nút, 4: nhấp nháy SETTING.
Nếu BTN2 và BTN3 cùng phát sự kiện, tăng trước rồi xác nhận.
Kiểm tra build bằng `ninja -C build/Debug`.

## Ràng buộc với code CubeMX

- Trong `src/main.c`, chỉ được sửa bên trong hàm `int main(void)`, tại các
  USER CODE blocks. Giữ nguyên mọi câu lệnh do CubeMX sinh ra trong hàm này.
- Không sửa include, biến toàn cục, prototype, callback hoặc bất kỳ hàm nào
  khác trong `main.c`, kể cả khi chúng nằm trong USER CODE blocks.
- Giữ nguyên code do CubeMX sinh ra. Logic ứng dụng và callback bổ sung phải
  được đặt trong các file ứng dụng riêng; không định nghĩa trùng callback đang có.
- Các lời gọi khởi tạo, điều phối và ba FSM trong tài liệu phải tuân theo giới
  hạn trên. Khi triển khai, kiểm tra cách cung cấp khai báo cho các lời gọi mới
  mà không sửa vùng include của `main.c`.
