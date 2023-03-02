import pandas as pd
import streamlit as st
import pywhatkit

from joblib import load
st.set_page_config(
    page_title="Multiplage App",
    page_icon=" "
)

def main():
    xgb_model = load('bph_predictor.json')
    html_temp="""
        <div style="background-color:lightblue;padding:16px">
            <h2 style="color:black;text-align:center;">BPH Detection using eXtreme Gradient Boosting</h2>
        </div>
    """
    st.markdown(html_temp,unsafe_allow_html=True)
    st.write('')
    st.write('')
    st.markdown("##### Enter the atmospheric conditions \n ")

    p1 = st.number_input("temperature in degree celsius",20,40,step=1)
    p2 = st.number_input("moisture in percentage",60,110,step=1)
    p3 = st.number_input("humidity in percentage",60,90,step=1)
    p4 = st.number_input("Nitrogen in Kilogram per hectare",110.00,160.00,step=1.00)
    p5 = st.number_input("Phosphorus in Kilogram per hectare",50.00,100.00,step=1.00)
    p6 = st.number_input("Potassium in Kilogram per hectare",50.00,100.00,step=1.00)

    user_input = pd.DataFrame({
        'temperature':p1,
        'moisture':p2,
        'humidity':p3,
        'nitrogen':p4,
        'phosphorous':p5,
        'potassium':p6,
    },index=[0])

    xgb_model.predict(user_input)

    try:
        if st.button('Predict'):
            pred = xgb_model.predict(user_input)
            #st.balloons()
            st.success("Probability of BPH = {:.3f}".format(pred[0]))
            if pred == 1.000:
                pywhatkit.sendwhatmsg('+918714267479','hello how are you')

    except:
        st.warning("Prediction error")

    if st.button('send whatsapp message'):
        pywhatkit.sendwhatmsg_instantly('+919072718041','Hi there is a presence of bph in your field kindly take the needful measures and inform agricultural officers as soon as possible',35,tab_close=True)
    
    
if __name__ == "__main__":
    main()