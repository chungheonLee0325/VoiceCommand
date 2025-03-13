import os
import json
import google.generativeai as genai
from dotenv import load_dotenv
from langchain_google_genai import ChatGoogleGenerativeAI
from langchain.prompts import PromptTemplate
from langchain.chains import LLMChain

def initialize_model():
    """환경 변수 로드 및 Google Gemini API 설정"""
    load_dotenv()
    api_key = os.getenv("GOOGLE_API_KEY")
    if api_key:
        genai.configure(api_key=api_key)
    else:
        raise ValueError("GOOGLE_GEMINI_API_KEY를 찾을 수 없습니다. .env 파일을 확인하세요.")

def load_json_data():
    """JSON 데이터 로드"""
    with open("./pals.json", "r", encoding="utf-8") as f:
        pals_data = json.load(f)
    with open("./items.json", "r", encoding="utf-8") as f:
        items_data = json.load(f)
    with open("./tasks.json", "r", encoding="utf-8") as f:
        tasks_data = json.load(f)
    return pals_data, items_data, tasks_data["tasks"]

def setup_llm_chain(pals_names, tasks_names, items_names):
    """LangChain 모델 및 프롬프트 설정"""
    prompt_template = PromptTemplate(
        input_variables=["user_input"],
        template="""
        You are an AI assistant that extracts structured game commands from natural language input.
        Given the user's command, determine the correct values for:
        - 'actor' (the Pal's name from the list: {pals})
        - 'work' (the appropriate task from the list: {tasks})
        - 'target' (the item/resource being acted upon from the list: {items})

        ### Example
        User: "도로롱 돌을 캐줘"
        Output:
        {{
            "actor": "Dororong",
            "work": "Mining",
            "target": "Stone",
            "forced": true
        }}

        ### Now extract the command:
        User: "{user_input}"
        Output:
        """
    )
    
    llm = ChatGoogleGenerativeAI(model="gemini-1.5-flash", temperature=0)
    return LLMChain(llm=llm, prompt=prompt_template)

def extract_command(user_input, llm_chain, pals_names, tasks_names, items_names):
    """자연어 명령을 게임 명령 JSON으로 변환"""
    response = llm_chain.run(user_input=user_input, pals=pals_names, tasks=tasks_names, items=items_names)
    print("LLM 응답 내용 확인:")
    print(response)
    
    if not response or response.strip() == "":
        print("LLM 응답이 비어 있습니다.")
        return {"error": "Empty response from LLM", "actor": "anyone", "work": None, "target": None, "forced": False}
    
    try:
        response = response.strip()
        if "{" in response and "}" in response:
            json_start = response.index("{")
            json_end = response.rindex("}") + 1
            response = response[json_start:json_end]
        response_json = json.loads(response)
        
        if "모두" in user_input:
            response_json["actor"] = "everyone"
        elif "아무나" in user_input:
            response_json["actor"] = "anyone"
        elif response_json.get("actor") is None:
            response_json["actor"] = "anyone"
        
        return response_json
    except json.JSONDecodeError as e:
        print(f"JSON 변환 실패! 오류: {e}")
        return {"error": "Invalid JSON response from LLM", "actor": "anyone", "work": None, "target": None, "forced": False}

def main():
    """모듈 실행 함수"""
    initialize_model()
    pals_data, items_data, tasks_data = load_json_data()
    pals_names = {pal["name"]: pal["name"] for pal in pals_data}
    tasks_names = {task["name"]: task["name"] for task in tasks_data}
    items_names = {item["name"]: item["name"] for item in items_data}
    llm_chain = setup_llm_chain(pals_names, tasks_names, items_names)
    
    user_command = "펭키 물좀 떠와"
    result = extract_command(user_command, llm_chain, pals_names, tasks_names, items_names)
    print(json.dumps(result, indent=4, ensure_ascii=False))

if __name__ == "__main__":
    main()
