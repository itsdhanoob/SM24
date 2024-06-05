import time


class MoodMonitor:
    def __init__(self, desired_temp, desired_moist, desired_intensity):
        self.desired_params = {
            "temp": desired_temp,
            "moist": desired_moist,
            "intensity": desired_intensity
        }
        self.current_params = {
            "temp": None,
            "moist": None,
            "intensity": None
        }
    
    def update_current_params(self, temp, moist, intensity):
        self.current_params["temp"] = temp
        self.current_params["moist"] = moist
        self.current_params["intensity"] = intensity

    def check_mood(self):
        if (self.current_params["temp"] == self.desired_params["temp"] and
            self.current_params["moist"] == self.desired_params["moist"] and
            self.current_params["intensity"] == self.desired_params["intensity"]):
            return "happy"

        elif (self.current_params["temp"] > self.desired_params['temp']): 
            return "too hot"
        elif (self.current_params["temp"] < self.desired_params['temp']): 
            return "too cold"    

        elif (self.current_params["moist"] > self.desired_params["moist"]):
              return "dont water me"
        elif (self.current_params["moist"] < self.desired_params["moist"]):
            return "please water me"

        elif (self.current_params["intensity"] > self.desired_params["intensity"]):
              return "i need shade"
        elif (self.current_params["intensity"] < self.desired_params["intensity"]):
            return "i need light"
        else:
            return "Sad"

    def start_monitoring(self):
        while True:
            mood = self.check_mood()
            print(f"Mood: {mood}")
            time.sleep(6 * 60 * 60)  # Sleep for 6 hours
