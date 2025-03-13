from fastapi import FastAPI, File, UploadFile, HTTPException
import requests
import tempfile
import os
from google.cloud import speech
from openai import OpenAI
from dotenv import load_dotenv

app = FastAPI()

# Google Cloud Speech-to-Text 설정
client = speech.SpeechClient()
GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")

@app.get("/")
def home():
    return {"message": "안녕하살법! 하지메 마시떼!"}

# def transcribe_audio(file_path: str) -> str:
#     with open(file_path, "rb") as audio_file:
#         content = audio_file.read()
#     audio = speech.RecognitionAudio(content=content)
#     config = speech.RecognitionConfig(
#         encoding=speech.RecognitionConfig.AudioEncoding.LINEAR16,
#         sample_rate_hertz=16000,
#         language_code="en-US"
#     )
#     response = client.recognize(config=config, audio=audio)
#     texts = [result.alternatives[0].transcript for result in response.results]
#     return " ".join(texts) if texts else ""

# def generate_unreal_code(text: str) -> str:
#     url = f"https://generativelanguage.googleapis.com/v1/models/gemini-pro:generateText?key={GEMINI_API_KEY}"
#     headers = {"Content-Type": "application/json"}
#     data = {
#         "prompt": { #프롬프트 수정
#             "text": f"Convert the following description into Unreal Engine C++ code:\n{text}"
#         },
#         "temperature": 0.7
#     }
#     response = requests.post(url, json=data, headers=headers)
#     if response.status_code == 200:
#         return response.json().get("candidates")[0]["output"]
#     else:
#         raise HTTPException(status_code=500, detail="Failed to generate Unreal code")


# @app.post("/convert-audio")
# async def convert_audio(file: UploadFile = File(...)):
#     try:
#         with tempfile.NamedTemporaryFile(delete=False, suffix=".wav") as tmp:
#             tmp.write(await file.read())
#             tmp_path = tmp.name
        
#         text = transcribe_audio(tmp_path)
#         os.remove(tmp_path)
        
#         if not text:
#             raise HTTPException(status_code=400, detail="Failed to transcribe audio")
        
#         unreal_code = generate_unreal_code(text)
#         return {"transcribed_text": text, "unreal_code": unreal_code}
#     except Exception as e:
#         raise HTTPException(status_code=500, detail=str(e))
