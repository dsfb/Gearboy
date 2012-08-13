#include "Input.h"
#include "Memory.h"
#include "Processor.h"

Input::Input(Memory* pMemory, Processor* pProcessor)
{
    m_pMemory = pMemory;
    m_pProcessor = pProcessor;
}

void Input::Init()
{
    m_JoypadState = 0xFF;
}

void Input::Reset()
{
    m_JoypadState = 0xFF;
}

u8 Input::GetJoyPadState()
{
    u8 reg = m_pMemory->Retrieve(0xFF00) ^ 0xFF;

    if (!IsSetBit(reg, 4))
    {
        u8 topJoypad = m_JoypadState >> 4;
        topJoypad |= 0xF0;
        reg &= topJoypad;
    }
    else if (!IsSetBit(reg, 5))
    {
        u8 bottomJoypad = m_JoypadState & 0xF;
        bottomJoypad |= 0xF0;
        reg &= bottomJoypad;
    }
    return reg;
}

void Input::KeyPressed(Gameboy_Keys key)
{
    bool previouslyUnset = false;

    if (!IsSetBit(m_JoypadState, key))
        previouslyUnset = true;

    m_JoypadState = UnsetBit(m_JoypadState, key);

    bool button = (key > 3);
    u8 reg = m_pMemory->Retrieve(0xFF00);
    bool requestInterupt = false;

    if (button && !IsSetBit(reg, 5))
        requestInterupt = true;
    else if (!button && !IsSetBit(reg, 4))
        requestInterupt = true;

    if (requestInterupt && !previouslyUnset)
        m_pProcessor->RequestInterrupt(Processor::Joypad_Interrupt);
}

void Input::KeyReleased(Gameboy_Keys key)
{
    m_JoypadState = SetBit(m_JoypadState, key);
}