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
| `global.h/c` | Trạng thái FSM hiện tại, cấu hình thời lượng, object đèn dùng chung và ID timer không trùng nhau. Không chứa logic vận hành. Các biến dùng chung được định nghĩa trong `.c` và khai báo `extern` trong `.h`. |
| `software_timer.h/c` | Nhận tick TIM2 và cung cấp cờ hết hạn. Không biết chế độ hay quy tắc đèn. |
| `button.h/c` | Chống dội và phát cờ nhấn một lần, không lặp khi giữ. Không tự đổi chế độ. |
| `traffic_light.h/c` | Quản lý từng object đèn: khởi tạo, đặt màu, đặt thời gian, tăng/giảm thời gian và cập nhật GPIO tương ứng. |
| `fsm_automatic.h/c` | Quản lý bốn pha hai hướng, đặt timer theo thời lượng mỗi pha và chuyển pha khi timer hết hạn. |
| `fsm_manual.h/c` | Quản lý pha hai hướng, chuyển pha theo sự kiện nút. |
| `fsm_setting.h/c` | Quản lý mục đang chỉnh, giá trị tạm, xác nhận cấu hình và nhấp nháy đèn. |
| `seg7_display.h/c` | Quản lý bộ đệm và quét hiển thị, phục vụ mọi chế độ. |
| `main.c` | Khởi tạo, gọi ba FSM cùng level và phục vụ hiển thị. |

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

Luồng xử lý trong main:

```text
while (1):
    FSM_Automatic_Run()
    FSM_Manual_Run()
    FSM_Setting_Run()
    FSM_ServiceOutputs()
```

Mỗi hàm Run có switch (state) ở cấp ngoài cùng, không nằm trong if
hoặc switch khác. Các case của AUTO/MANUAL dùng chung logic chuyển pha;
SETTING có các case riêng SET_RED, SET_YELLOW, SET_GREEN. Không có orchestrator.

## Chuyển chế độ và sự kiện

- Một enum dùng chung chứa bốn trạng thái AUTO, bốn MANUAL và ba SETTING.
- Mỗi FSM tự đọc/xóa cờ nút trong case và gọi Init của chế độ đích khi BTN1
  được nhấn. BTN1 có ưu tiên cao hơn BTN2/BTN3.
- Init đặt trạng thái, đầu ra và timer; xóa mọi cờ nút để tránh truyền lần
  nhấn sang chế độ mới. Cấu hình thời lượng đã lưu được giữ nguyên.
- FSM tiêu thụ cờ nút đã xử lý; Init xóa cờ chưa dùng khi chuyển chế độ.
- Ba FSM chạy tuần tự; chế độ mới có thể chạy ngay trong cùng lượt nếu
  lời gọi Run của nó nằm phía sau. Không còn cờ nhấn để xử lý lần thứ hai.
- Quét hiển thị và đèn báo chạy sau cả ba FSM. TIM2 cập nhật software timer và quét nút theo chu kỳ cố định.

## Hành vi cơ sở đề xuất

- BTN1: `AUTO → MANUAL → SETTING → AUTO`.
- AUTO: tự chạy chuỗi pha và hiển thị thời gian còn lại.
- MANUAL: BTN2 chuyển một pha cho mỗi lần nhấn, không lặp khi giữ; BTN3 chưa có chức năng. Hiển thị thời lượng ban đầu của pha và giữ nguyên đến lần chuyển pha tiếp theo.
- SETTING: BTN2 tăng giá trị một lần mỗi lần nhấn, giữ nút không lặp; BTN3 lưu rồi chuyển mục `đỏ → vàng → xanh`;
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
`src/fsm.c` cung cấp khởi tạo, quét/xóa cờ nút, cập nhật bộ đệm
và thao tác xuất pha dùng chung. Khai báo API nằm trong `src/fsm.h`.
`main` gọi ba FSM ngang hàng; chỉ sửa USER CODE trong thân hàm `main`.
CMake và self-check đã được cập nhật. Cấu hình lưu trong RAM, reset về xanh 2 s, vàng 3 s.
AUTO giữ cách hiển thị cũ N-1..0; SETTING hiển thị 02/03/04 và giá trị đang sửa.
AUTO suy ra thời gian hiển thị từ timer pha, không giảm bộ đếm của từng đèn.
Timer 0: AUTO, 1: quét LED 7 đoạn, 2: đèn báo, 3: nhấp nháy SETTING. Quét nút trực tiếp trong tick TIM2.
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

