from dotenv import load_dotenv
import os
import whisper
from langchain_core.output_parsers import PydanticOutputParser
from langchain_core.prompts import PromptTemplate
from pydantic import BaseModel, Field


GEMINI_API_KEY = os.getenv('GOOGLE_API_KEY')
stt_model=whisper.load_model('base')


#### 오디오 STT 코드 부분과, 입력 받은 텍스트를 LLM을 거쳐 json으로 전환해주는 함수

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
    


#Pydantic 객체 프롬프트/ 파서 등 llm에 필요한 class및 메서드
class JSONparser(BaseModel):
    actor: str = Field(description="명령할 팰 ['Lamball', 'Everyone' , 'Anyone']")
    work: str =Field(...,description="수행할 액션 ['Lumbering', 'Mining']")
    target: str = Field(description="액션을 수행할 객체 ['Ore','Stone','Anyone','Tree']")
    forced: bool = Field(description="강제성이나 긴급함 여부 [True,False]")
