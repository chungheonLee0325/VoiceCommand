from dotenv import load_dotenv

GEMINI_API_KEY = load_dotenv('GOOGLE_API_KEY')

def transcribe_audio(file_path: str) -> str:
    with open(file_path, "rb") as audio_file:
        content = audio_file.read()
    audio = speech.RecognitionAudio(content=content)
    config = speech.RecognitionConfig(
        encoding=speech.RecognitionConfig.AudioEncoding.LINEAR16,
        sample_rate_hertz=16000,
        language_code="en-US"
    )
    response = client.recognize(config=config, audio=audio)
    texts = [result.alternatives[0].transcript for result in response.results]
    return " ".join(texts) if texts else ""

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