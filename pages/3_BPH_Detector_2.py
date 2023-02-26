import pandas as pd
import streamlit as st
from joblib import load
st.set_page_config(
    page_title="Multiplage App",
    page_icon=" "
)

def main():
    xgb_model = load('bph_predictor2.json')
    html_temp="""
        <div style="background-color:lightblue;padding:16px">
            <h2 style="color:black;text-align:center;">BPH Detection using eXtreme Gradient Boosting</h2>
        </div>
    """
    st.markdown(html_temp,unsafe_allow_html=True)
    st.write('')
    st.write('')
    st.markdown("##### Enter the values by PIR and TCS3200 sensor. \n ")

    p1 = st.number_input("color in hexadecimal",20,40,step=1)
    p2 = st.number_input("IR Value",60,110,step=1)

    user_input = pd.DataFrame({
        'temperature':p1,
        'humidity':p2,
    },index=[0])

    xgb_model.predict(user_input)
    try:
        if st.button('Predict'):
            pred = xgb_model.predict(user_input)
            #st.balloons()
            st.success("Probability of BPH = {:.3f}".format(pred[0]))
    except:
        st.warning("Prediction error")
    
if __name__ == "__main__":
    main()