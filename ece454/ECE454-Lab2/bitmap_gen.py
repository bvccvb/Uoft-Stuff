from PIL import Image
import random
import csv

def create_bmp(dim = 50, bg_border = 50):
	img_size = dim + bg_border
	print(img_size)
	img  = Image.new('RGB', (img_size, img_size),  (255, 255, 255))
	img_load = img.load()

	for i in range(bg_border, img.size[0] - bg_border):
		for j in range(bg_border, img.size[1] - bg_border):
			R, G, B = random.randint(0,255), random.randint(0,255), random.randint(0,255)
			img_load[i,j] = (R, G, B)
			
	img.save("test.bmp")

def create_sensor_inputs(dim = 50, border = 50, num_commands = 2):
	# keys = ['W', 'A', 'S', 'D', 'CW', 'CCW', 'MX', 'MY']
	keys = ['CW']
	# keys = ['MX']
	print(keys)
	# key_translate = keys[0:4]
	# print(key_translate)
	img_size = dim + border

	top_pixel = border
	left_pixel = border
	bot_pixel = border
	right_pixel = border

	min_rotations = 1
	max_rotations = 1

	with open("test.csv", 'w') as f:
		writer = csv.writer(f)

		for i in range(num_commands):
			key = random.choice(keys)
			val = 0
			if key == 'W':
				val = random.randint(0, 5)
				top_pixel -= val
				bot_pixel += val

			elif key == 'A':
				val = random.randint(0, right_pixel)
				left_pixel -= val
				right_pixel += val

			elif key == 'S':
				val = random.randint(0, bot_pixel)
				bot_pixel -= val
				top_pixel += val

			elif key == 'D':
				val = random.randint(0, right_pixel)
				right_pixel -= val
				left_pixel += val

			elif key == 'CW' or key == 'CCW':
				val = random.randint(min_rotations, max_rotations)

			elif key == 'MX' or key == 'MY':
				val = random.randint(0,10)

			# print("KEY: {} VAL: {} \n top: {} bottom: {} right: {} left: {}".format(key, val, top_pixel, bot_pixel, right_pixel, left_pixel))

			writer.writerow([key, val])





if __name__ == '__main__':
	image_dim = 1500
	border_padding = 400
	num_commands = 100
	create_bmp(image_dim, border_padding)
	create_sensor_inputs(image_dim, border_padding, num_commands)
	print("Generated BMP & CSV");
	