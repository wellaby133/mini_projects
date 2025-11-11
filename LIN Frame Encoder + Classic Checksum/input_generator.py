import random

def generate_random_message_hex():
    msg_id = random.randint(0, 255)
    msg_len = random.randint(1, 8)
    msg_bytes = [random.randint(0, 255) for _ in range(msg_len)]
    packet = [msg_id, msg_len] + msg_bytes
    return ' '.join(f'{b:02X}' for b in packet)

# === CONFIGURATION ===
OUTPUT_FILE = "inputs.txt"
NUM_MESSAGES = 5  # Change as needed
# =====================

with open(OUTPUT_FILE, 'w') as f:
    for _ in range(NUM_MESSAGES):
        hex_line = generate_random_message_hex()
        f.write(hex_line + '\n')