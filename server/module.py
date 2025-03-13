from dotenv import load_dotenv
import os
import whisper

GEMINI_API_KEY = os.getenv('GOOGLE_API_KEY')
stt_model=whisper.load_model('base')

def transcribe_audio(file_path: str) -> str:
    result = stt_model.transcribe(file_path)
    return result["text"].strip()

def generate_unreal_code(text: str) -> str:
    url = f"https://generativelanguage.googleapis.com/v1/models/gemini-pro:generateText?key={GEMINI_API_KEY}"
    headers = {"Content-Type": "application/json"}
    data = {
        "prompt": { #프롬프트 수정
            "text": f"Convert the following description into Unreal Engine C++ code:\n{text}"
        },
        "temperature": 0.7
    }
    response = requests.post(url, json=data, headers=headers)
    if response.status_code == 200:
        return response.json().get("candidates")[0]["output"]
    else:
        raise HTTPException(status_code=500, detail="Failed to generate Unreal code")