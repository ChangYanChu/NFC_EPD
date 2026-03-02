import cv2
import numpy as np

pal_arr = [
    # 黑白调色板 (索引0)
    [(0, 0, 0), (255, 255, 255)],
    # 黑红调色板 (索引1) - 注意：这里只有黑色和红色
    [(0, 0, 0), (0, 0, 255)],
    # 黑、红、白三色调色板 (索引2)
    [(0, 0, 0), (0, 0, 255), (255, 255, 255)]
]

# EPD 显示配置
epd_arr = [
    # 格式: [宽度, 高度, 调色板索引]
    [800, 600, 2],  # 使用三色调色板
]

def get_near(r, g, b, is_red_palette=True):
    """
    改进的颜色判断函数
    对于红黑调色板，只有当像素与红色相似时才返回红色索引
    """
    if not is_red_palette:
        # 黑白调色板：简单亮度判断
        gray = 0.299 * r + 0.587 * g + 0.114 * b
        return 0 if gray < 128 else 1

    # 对于红黑调色板，进行更复杂的颜色判断

    # 计算亮度
    gray = 0.299 * r + 0.587 * g + 0.114 * b

    # 计算与纯红色的相似度
    red_similarity = r - max(g, b)

    # 计算与纯黑色的相似度
    black_similarity = 255 - gray  # 值越大表示越接近黑色

    # 判断逻辑：
    # - 如果很暗，使用黑色
    # - 如果与红色相似且不是太暗或太亮，使用红色
    # - 否则根据亮度使用黑色或白色

    if gray < 60:  # 很暗的区域
        return 0  # 黑色
    elif gray > 200:  # 很亮的区域
        return 2  # 白色
    elif red_similarity > 30 and 80 < gray < 180:  # 与红色相似且中等亮度
        return 1  # 红色
    else:
        # 其他情况根据亮度决定
        return 0 if gray < 128 else 2

def add_val(err_arr, r, g, b, factor):
    """
    将误差值乘以系数后加到误差数组中
    """
    factor /= 16.0
    return [
        err_arr[0] + r * factor,
        err_arr[1] + g * factor,
        err_arr[2] + b * factor
    ]

def proc_img(image,x=0, y=0, w=None, h=None):
    """
    处理图像的主函数

    参数:
    - image: 输入图像
    - is_red: 是否使用红黑调色板 (True) 或黑白调色板 (False)
    - x, y: 处理区域的起始坐标
    - w, h: 处理区域的宽度和高度
    """

    # 获取图像尺寸
    sH, sW = image.shape[:2]

    # 设置默认处理区域
    if w is None:
        w = sW
    if h is None:
        h = sH

    # 选择调色板
    epd_ind = 0  # 使用第一个EPD配置
    pal_ind = epd_arr[epd_ind][2]

    cur_pal = pal_arr[pal_ind]

    # 创建输出图像
    output = np.zeros((h, w, 3), dtype=np.uint8)

    err_arr = [np.zeros((w, 3), dtype=np.float32) for _ in range(2)]
    a_ind = 0
    b_ind = 1

    for j in range(h):
        y_pos = y + j
        if y_pos < 0 or y_pos >= sH:
            # 超出边界，使用棋盘格填充
            for i in range(w):
                color_idx = 0 if (i + j) % 2 == 0 else 1
                output[j, i] = cur_pal[color_idx]
            continue

        # 交换误差数组索引
        a_ind, b_ind = b_ind, a_ind
        # 重置当前行的误差
        err_arr[b_ind] = np.zeros((w, 3), dtype=np.float32)

        for i in range(w):
            x_pos = x + i
            if x_pos < 0 or x_pos >= sW:
                # 超出边界，使用棋盘格填充
                color_idx = 0 if (i + j) % 2 == 0 else 1
                output[j, i] = cur_pal[color_idx]
                continue

            # 获取像素值和当前误差
            pixel = image[y_pos, x_pos]
            b, g, r = pixel  # OpenCV使用BGR格式
            old_err = err_arr[a_ind][i]

            # 添加误差到像素值
            r_new = r + old_err[0]
            g_new = g + old_err[1]
            b_new = b + old_err[2]

            # 钳制值到0-255范围
            r_new = np.clip(r_new, 0, 255)
            g_new = np.clip(g_new, 0, 255)
            b_new = np.clip(b_new, 0, 255)

            # 计算最接近的颜色索引
            color_idx = get_near(r_new, g_new, b_new)
            color_val = cur_pal[color_idx]

            # 设置输出像素
            output[j, i] = color_val

            # 计算误差
            r_err = r_new - color_val[2]  # 注意：OpenCV是BGR，红色在索引2
            g_err = g_new - color_val[1]
            b_err = b_new - color_val[0]

            # 扩散误差到相邻像素
            if i == 0:
                # 第一列
                if i < w - 1:
                    err_arr[b_ind][i] += np.array([r_err, g_err, b_err]) * (7.0 / 16.0)
                    err_arr[b_ind][i+1] += np.array([r_err, g_err, b_err]) * (2.0 / 16.0)
                    err_arr[a_ind][i+1] += np.array([r_err, g_err, b_err]) * (7.0 / 16.0)
            elif i == w - 1:
                # 最后一列
                err_arr[b_ind][i-1] += np.array([r_err, g_err, b_err]) * (7.0 / 16.0)
                err_arr[b_ind][i] += np.array([r_err, g_err, b_err]) * (9.0 / 16.0)
            else:
                # 中间列
                err_arr[b_ind][i-1] += np.array([r_err, g_err, b_err]) * (3.0 / 16.0)
                err_arr[b_ind][i] += np.array([r_err, g_err, b_err]) * (5.0 / 16.0)
                err_arr[b_ind][i+1] += np.array([r_err, g_err, b_err]) * (1.0 / 16.0)
                err_arr[a_ind][i+1] += np.array([r_err, g_err, b_err]) * (7.0 / 16.0)

    return output

def demo():
    width = 104
    heigh = 212

    width = 128
    heigh = 296

    # width = 128
    # heigh = 250
    test_image = cv2.imread("test.png")
    # test_image = cv2.imread("images2.jpg")
    # 显示原始图像
    height_,width_ = test_image.shape[:2]
    print(height_,width_)
    if width_ < height_:
        test_image = cv2.resize(test_image,(width,heigh))

    else:
        test_image = cv2.resize(test_image,(heigh,width))
        test_image = cv2.transpose(test_image)

    # 误差扩散 + 红黑
    resultImg = proc_img(test_image)
    cv2.imwrite("result.png", resultImg)
    print("抖动处理完成，结果已保存到 result.png")

    R = np.reshape(resultImg,(width*heigh,-1))
    red = [0]*(width*heigh//8)
    black = [0]*(width*heigh//8)
    # print(len(resultImg[2]))
    for index,i in enumerate(R):
        p_i = index // 8 
        bit_i = 7 - (index % 8)
        # bit_i = index % 8
        mask_y = ~(1 << bit_i) & 0xff  # 用作与 &
        mask_h = 1 << bit_i & 0xff # 用作或 |
        # print(bin(mask_y), bin(mask_h))
        s = int(i[0]) + int(i[1]) + int(i[2])  # 避免 uint8 溢出
        if s == 765:
            #white 红黑都置为1
            black[p_i] = black[p_i] | mask_h
            red[p_i] = red[p_i] | mask_h
        elif s == 255:
            #red 红置为0 黑置为1
            black[p_i] = black[p_i] | mask_h
            red[p_i] = red[p_i] & mask_y
        elif s == 0:
        # black 红置为1 黑置为0
            black[p_i] = black[p_i] & mask_y
            red[p_i] = red[p_i] | mask_h
        else:
            print(f"error: unexpected color sum={s}, pixel={i}")

    # R = black 
    R = black + red
    data = b"".join([i.to_bytes(1,byteorder='little') for i in R])
    with open("img.data", "wb") as f:
        f.write(data)

demo()