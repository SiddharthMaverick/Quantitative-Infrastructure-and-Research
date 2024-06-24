import os
from datetime import datetime, timedelta

def get_symbols(expiry_date_str):
    expiry_date = datetime.strptime(expiry_date_str, "%Y%m%d").date()
    day_before_expiry = expiry_date - timedelta(days=1)
    
    
    expiry_date_str = expiry_date.strftime("%Y%m%d")
    day_before_expiry_str = day_before_expiry.strftime("%Y%m%d")
    

    expiry_d=expiry_date.strftime("%d%m%y")
    day_expiry_before_d=expiry_date.strftime("%d%m%y")
    
    
    expiry_folder_path = f"/home/ubuntu/data1/{expiry_date_str}"
    day_before_expiry_folder_path = f"/home/ubuntu/data1/{day_before_expiry_str}"
    
    expiry_files = os.listdir(expiry_folder_path)
    day_before_expiry_files = os.listdir(day_before_expiry_folder_path)

    
    expiry_symbols = [file.split(".")[0] for file in expiry_files if file.endswith(f"{expiry_d}.csv")]
    day_before_expiry_symbols = [file.split(".")[0] for file in day_before_expiry_files if file.endswith(f"{day_expiry_before_d}.csv")]

    final_list=[file for file in expiry_symbols if file in day_before_expiry_symbols]
    final_list.append("MARK:BTCUSDT")
    
    return final_list

def write_config(symbols,start_date,end_date):
    with open("config.py", "w") as f:
        f.write("symbols = [\n")
        for symbol in symbols:
                f.write(f"    '{symbol}',\n")
        f.write("]\n")

        f.write(f"simStartDate={start_date}")
        f.write("\n")
        f.write(f"simEndDate={end_date}")

if __name__ == "__main__":
    expiry_date_input = input("Enter the expiry date (YYYYMMDD): ")
    final_list= get_symbols(expiry_date_input)
    all_symbols = set(final_list)
    start_date=input("Enter Start Date : ")
    end_date=input("Enter End Date : ")
    
    write_config(all_symbols,start_date,end_date)
    print("Symbols list saved in config.py.")
