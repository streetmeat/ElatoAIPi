import { SupabaseClient } from '@supabase/supabase-js';

declare global {
    interface IConversation {
        conversation_id: string;
        role: 'user' | 'assistant';
        content: string;
        user_id: string;
        is_sensitive: boolean;
        personality_key: string;
        created_at: string;
    }

    interface IPayload {
        user: IUser;
        supabase: SupabaseClient;
        timestamp: string;
    }

    interface IDevice {
        device_id: string;
        volume: number;
        is_ota: boolean;
        is_reset: boolean;
        mac_address: string;
        user_code: string;
    }

    interface IPersonality {
        personality_id: string;
        is_doctor: boolean;
        is_child_voice: boolean;
        is_story: boolean;
        key: string;
        oai_voice: 'ash' | 'alloy' | 'echo' | 'shimmer' | 'ballad' | 'coral' | 'sage' | 'verse';
        voice_description: string;
        title: string;
        subtitle: string;
        short_description: string;
        character_prompt: string;
        voice_prompt: string;
        creator_id: string | null;
    }

    interface ILanguage {
        language_id: string;
        code: string;
        name: string;
        flag: string;
    }

    interface IDoctorMetadata {
        doctor_name: string;
        specialization: string;
        hospital_name: string;
        favorite_phrases: string;
    }

    interface IUserMetadata {}
    interface IBusinessMetadata {}

    type UserInfo =
        | {
            user_type: 'user';
            user_metadata: IUserMetadata;
        }
        | {
            user_type: 'doctor';
            user_metadata: IDoctorMetadata;
        }
        | {
            user_type: 'business';
            user_metadata: IBusinessMetadata;
        };

    interface IUser {
        user_id: string;
        avatar_url: string;
        is_premium: boolean;
        email: string;
        supervisor_name: string;
        supervisee_name: string;
        supervisee_persona: string;
        supervisee_age: number;
        personality_id: string;
        personality?: IPersonality;
        language: ILanguage;
        language_code: string;
        session_time: number;
        user_info: UserInfo;
        device_id: string;
        device?: IDevice;
    }
}
